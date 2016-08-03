// https://forum.openframeworks.cc/t/how-about-a-yesno-dialogue-box/14001

#include "ofConstants.h"
#include "ofYesNoDialog.h"

#ifdef TARGET_OSX
	// ofSystemUtils.cpp is configured to build as
	// objective-c++ so as able to use Cocoa dialog panels
	// This is done with this compiler flag
	//		-x objective-c++
	// http://www.yakyak.org/viewtopic.php?p=1475838&sid=1e9dcb5c9fd652a6695ac00c5e957822#p1475838

	#include <Cocoa/Cocoa.h>
#endif

//------------------------------------------------------------------------------
bool ofSystemYesNoDialog(string title,string message){


#ifdef TARGET_WIN32
    int length = strlen(title.c_str());
    wchar_t * widearray = new wchar_t[length+1];
    memset(widearray, 0, sizeof(wchar_t)*(length+1));
    mbstowcs(widearray, title.c_str(), length);
    int length2 = strlen(message.c_str());
    wchar_t * widearray2 = new wchar_t[length2+1];
    memset(widearray2, 0, sizeof(wchar_t)*(length2+1));
    mbstowcs(widearray2, message.c_str(), length2);
    int dialogueResult = MessageBoxW(NULL, widearray2, widearray, MB_OKCANCEL);
    delete widearray;
    delete widearray2;
	return dialogueResult == 1;
#endif


#ifdef TARGET_OSX
    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:[NSString stringWithCString:title.c_str()
           encoding:NSUTF8StringEncoding]];
    [alert setInformativeText:[NSString stringWithCString:message.c_str()
           encoding:NSUTF8StringEncoding]];
    
    NSInteger returnCode = [alert runModal];
    
    return returnCode == NSAlertFirstButtonReturn;

#endif
//only works for pc and mac // shouldn't be hard to do the linux version though
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
    initGTK();
    GtkWidget* dialog = gtk_message_dialog_new (NULL, (GtkDialogFlags) 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", errorMessage.c_str());
    gtk_dialog_run (GTK_DIALOG (dialog));
    startGTK(dialog);
#endif

#ifdef TARGET_ANDROID
    ofxAndroidAlertBox(errorMessage);
#endif
}