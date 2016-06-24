#pragma once

#include <GRT/GRT.h>

/**
 @brief TrainingSampleCheckerResult indicates the result of a check of a training sample.

 There are three supported result: SUCCESS, WARNING, FAILURE. An optional string
 description can be supplied to better assist user figuring out why the
 training sample fails. This class is used as the return value for the functions
 passed to useTrainingSampleChecker().
 
 You can create a TrainingSampleCheckerResult like this:
 
     return TrainingSampleCheckerResult(TrainingSampleCheckerResult::FAILURE, "Error: Something went wrong.");
 
 Or just use the Result directly if you don't want to supply a custom message:
 
     return TrainingSampleCheckerResult::SUCCESS; // use default message
*/
class TrainingSampleCheckerResult {
  public:
    enum Result {
        SUCCESS,
        WARNING,
        FAILURE,
    };

    TrainingSampleCheckerResult(Result result);
    TrainingSampleCheckerResult(Result result, string message);

    Result getResult() const { return result_; }
    string getMessage() const { return result_message_; }

  private:
    Result result_;
    string result_message_;

    static const string kDefaultSuccessMessage;
    static const string kDefaultWarningMessage;
    static const string kDefaultFailureMessage;
};

/**
 @brief function that takes a constant reference to a GRT MatrixDouble and
 returns a TrainingSampleCheckerResult
 
 The training sample checker functions passed to useTrainingSampleChecker()
 take a const reference to a GRT MatrixDouble and return a 
 TrainingSampleCheckerResult, e.g.

     TrainingSampleCheckerResult myChecker(const MatrixDouble &data)
 */
typedef TrainingSampleCheckerResult (*TrainingSampleChecker)(const GRT::MatrixDouble &);

/**
 @brief Register a function for checking training samples.
 
 The TrainingSampleChecker specified here will be called on each new sample
 of training data collected by the user. The result, indicated by the
 TrainingSampleCheckerResult returned, will be shown to the user.
 
 Here's an example of how you might use this function:
 
     TrainingSampleCheckerResult myChecker(const MatrixDouble &data) {
         if (data.getNumRows() == 0) {
             return TrainingSampleCheckerResult(
                 TrainingSampleCheckerResult::FAILURE,
                 "Error: Training sample doesn't contain any data.");
         }
         if (data.getNumRows() == 1) {
             return TrainingSampleCheckerResult(
                 TrainingSampleCheckerResult::WARNING,
                 "Warning: Sample only contains one data point.");
         }

         return TrainingSampleCheckerResult::SUCCESS; // use default message
     }
     
     void setup() {
         useTrainingSampleChecker(myChecker);
     }
 
 Note that only one TrainingSampleChecker can be active at any time.
 Subsequent calls to this function will replace the previously-registered
 checker.
 
 @param checker the function to be called on the user's training samples
 */
void useTrainingSampleChecker(TrainingSampleChecker checker);

/**
 @brief Provide the user with custom advice on collecting training data.
 
 This advice will be shown in the training tab of the interface. If supplied,
 it will override the default, per-classifier advice provided by ESP.
 
 You may want to provide advice on:
   - the amount of training data required
   - the effect of gathering additional training data
   - the effect of individual bad training samples
   - what good sample look like (although see useTrainingSampleChecker() for
     a programmatic means of providing the user with feedback on the quality of
     individual training samples)
   - etc.
 
 On the other hand, try to keep the advice relatively brief, as it will take up
 space on the training tab of the ESP interface.
 
 The string will be automatically wrapped at the edge of the screen. No markup
 or formatting (including explicit line breaks) supported.
 
 @param advice the advice to show to the user
 */
void useTrainingDataAdvice(string advice);