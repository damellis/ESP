#include "ostream.h"
#include "ofUtils.h"

#ifdef TARGET_WIN32
#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif

#include <thread>
#include "ofxTCPClient.h"

extern "C" {
#include <ep/ep.h>
#include <ep/ep_dbg.h>
#include <ep/ep_app.h>
#include <ep/ep_time.h>
#include <gdp/gdp.h>
#include <event2/buffer.h>
}

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

GDPOutputStream::GDPOutputStream(const char *log_name, bool asynch) :
    log_name_(log_name), asynch_(asynch) {}

bool GDPOutputStream::start()
{
    gdp_name_t gcliname;
    int opt;
    EP_STAT estat;
    char *gdpd_addr = NULL;
    bool show_usage = false;
    bool one_record = false;
    char *log_file_name = NULL;
    char *signing_key_file = NULL;
    gdp_gcl_open_info_t *info;

    // initialize the GDP library
    estat = gdp_init(gdpd_addr);
    if (!EP_STAT_ISOK(estat))
    {
        ep_app_error("GDP Initialization failed");
        goto fail0;
    }

    // allow thread to settle to avoid interspersed debug output
    ep_time_nanosleep(INT64_C(100000000));

    // set up any open information
    info = gdp_gcl_open_info_new();

    // open a GCL with the provided name
    gdp_parse_name(log_name_, gcliname);
    estat = gdp_gcl_open(gcliname, GDP_MODE_AO, info, &gcl);
    EP_STAT_CHECK(estat, goto fail1);

    gdp_pname_t pname;

    // dump the internal version of the GCL to facilitate testing
    ofLogNotice() << "GDPname: "
        << gdp_printable_name(*gdp_gcl_getname(gcl), pname)
        << " (" << gdp_gcl_getnrecs(gcl) << " recs)";
fail1:
    if (info != NULL)
            gdp_gcl_open_info_free(info);

fail0:
    has_started_ = true;
    if (!EP_STAT_ISOK(estat))
    {
        char buf[200];

        ofLogWarning() << "Error connecting to GDP: " <<
            ep_stat_tostr(estat, buf, sizeof buf);
        has_started_ = false;
    }

    return has_started_;
}

static const char	*EventTypes[] =
{
    "Free (internal use)",
    "Data",
    "End of Subscription",
    "Shutdown",
    "Asynchronous Status",
};

void
showstat(gdp_event_t *gev)
{
    int evtype = gdp_event_gettype(gev);
    EP_STAT estat = gdp_event_getstat(gev);
    gdp_datum_t *d = gdp_event_getdatum(gev);
    char ebuf[100];
    char tbuf[20];
    const char *evname;

    if (evtype < 0 || evtype >= sizeof EventTypes / sizeof EventTypes[0])
    {
        snprintf(tbuf, sizeof tbuf, "%d", evtype);
        evname = tbuf;
    }
    else
    {
        evname = EventTypes[evtype];
    }

    ofLogVerbose("[GDP]", "Asynchronous event type %s:\n"
                    "\trecno %" PRIgdp_recno ", stat %s\n",
                    evname,
                    gdp_datum_getrecno(d),
                    ep_stat_tostr(estat, ebuf, sizeof ebuf));

    gdp_event_free(gev);
}

void GDPOutputStream::onReceive(uint32_t label) {
    if (!has_started_) return;

    char buf[100];
    int n = snprintf(buf, 100, "%u", label);
  
    // we need a place to buffer the input
    gdp_datum_t *datum = gdp_datum_new();
    gdp_buf_write(gdp_datum_getbuf(datum), buf, n);
    EP_STAT estat;
    
    if (asynch_)
    {
        estat = gdp_gcl_append_async(gcl, datum, showstat, NULL);
        EP_STAT_CHECK(estat, ofLogWarning("[GDP]", "Error writing to log asynchronously"));

        // return value will be printed asynchronously
    }
    else
    {
        estat = gdp_gcl_append(gcl, datum);

        if (!EP_STAT_ISOK(estat))
        {
            char ebuf[100];
            ofLogWarning("[GDP]", "Append error: %s",
                                    ep_stat_tostr(estat, ebuf, sizeof ebuf));
        }
    }
    
    // OK, all done.  Free our resources and exit
    gdp_datum_free(datum);
}

void MacOSKeyboardOStream::sendKey(char c) {
#if __APPLE__
    if (ofGetElapsedTimeMillis() < elapsed_time_ + kGracePeriod) {
        return;
    }
    elapsed_time_ = ofGetElapsedTimeMillis();

    // Get the process number for the front application.
    ProcessSerialNumber psn = { 0, kNoProcess };
    GetFrontProcess( &psn );

    UniChar uni_char = c;
    CGEventRef key_down = CGEventCreateKeyboardEvent(NULL, 0, true);
    CGEventRef key_up = CGEventCreateKeyboardEvent(NULL, 0, false);
    CGEventKeyboardSetUnicodeString(key_down, 1, &uni_char);
    CGEventKeyboardSetUnicodeString(key_up, 1, &uni_char);
    CGEventPostToPSN(&psn, key_down);
    CGEventPostToPSN(&psn, key_up);
    CFRelease(key_down);
    CFRelease(key_up);
#endif
}

void MacOSKeyboardOStream::sendKeyCode(uint16_t key) {
#if __APPLE__
    if (ofGetElapsedTimeMillis() < elapsed_time_ + kGracePeriod) {
        return;
    }
    elapsed_time_ = ofGetElapsedTimeMillis();

    // Get the process number for the front application.
    ProcessSerialNumber psn = { 0, kNoProcess };
    GetFrontProcess( &psn );

    CGEventRef key_down = CGEventCreateKeyboardEvent(NULL, key, true);
    CGEventRef key_up = CGEventCreateKeyboardEvent(NULL, key, false);
    CGEventPostToPSN(&psn, key_down);
    CGEventPostToPSN(&psn, key_up);
    CFRelease(key_down);
    CFRelease(key_up);
#endif
}

void MacOSKeyboardOStream::sendString(const std::string& str) {
#if __APPLE__
    // Get the process number for the front application.
    ProcessSerialNumber psn = { 0, kNoProcess };
    GetFrontProcess( &psn );

    UniChar s[str.length()];
    for (uint32_t i = 0; i < str.length(); i++) {
        s[i] = str[i];
    }

    CGEventRef e = CGEventCreateKeyboardEvent(NULL, 0, true);
    CGEventKeyboardSetUnicodeString(e, str.length(), s);
    CGEventPostToPSN(&psn, e);
    CFRelease(e);
#endif
}

void MacOSMouseOStream::clickMouse(pair<uint32_t, uint32_t> mouse) {
#if __APPLE__
    if (ofGetElapsedTimeMillis() < elapsed_time_ + kGracePeriod) {
        return;
    }
    elapsed_time_ = ofGetElapsedTimeMillis();

    doubleClick(mouse);
#endif
}

void MacOSMouseOStream::doubleClick(pair<uint32_t, uint32_t> mouse, int clickCount) {
#if __APPLE__
    CGPoint point = CGPointMake(mouse.first, mouse.second);
    CGEventRef theEvent = CGEventCreateMouseEvent(
        NULL, kCGEventLeftMouseDown, point, kCGMouseButtonLeft);

    ProcessSerialNumber psn = { 0, kNoProcess };
    GetFrontProcess( &psn );

    CGEventSetIntegerValueField(theEvent, kCGMouseEventClickState, clickCount);
    CGEventPostToPSN(&psn, theEvent);
    CGEventSetType(theEvent, kCGEventLeftMouseUp);
    CGEventPostToPSN(&psn, theEvent);
    CGEventSetType(theEvent, kCGEventLeftMouseDown);
    CGEventPostToPSN(&psn, theEvent);
    CGEventSetType(theEvent, kCGEventLeftMouseUp);
    CGEventPostToPSN(&psn, theEvent);
    CFRelease(theEvent);
#endif
}

bool TcpOStream::start() {
    if (client_ == nullptr) {
        client_ = new ofxTCPClient();
    }
    has_started_ = client_->setup(server_, port_);
    return has_started_;
}

void TcpOStream::sendString(const string& tosend) {
    if (client_ != nullptr && client_->isConnected()) {
        client_->sendRaw(tosend);
    }

    // If not connected, we start a new thread that tries to connect
    if (!client_->isConnected() && !is_in_retry_) {
        is_in_retry_ = true;
        auto t = std::thread([this] () {
            while (!this->client_->setup(server_, port_)) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            is_in_retry_ = false;
        });
        t.detach();
    }
}
