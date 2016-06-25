#include "training.h"
#include "ofApp.h"

const string TrainingSampleCheckerResult::kDefaultSuccessMessage = "Success";
const string TrainingSampleCheckerResult::kDefaultWarningMessage = "Warning";
const string TrainingSampleCheckerResult::kDefaultFailureMessage = "Error";

TrainingSampleCheckerResult::TrainingSampleCheckerResult(Result result) : result_(result) {
    switch (result) {
        case SUCCESS:
            result_message_ = kDefaultSuccessMessage;
            break;
        case WARNING:
            result_message_ = kDefaultWarningMessage;
            break;
        case FAILURE:
            result_message_ = kDefaultFailureMessage;
            break;
    }
}

TrainingSampleCheckerResult::TrainingSampleCheckerResult(Result result, string message)
        : result_(result), result_message_(std::move(message)) {
}

void useTrainingSampleChecker(TrainingSampleChecker checker) {
    ((ofApp *) ofGetAppPtr())->useTrainingSampleChecker(checker);
}

void useTrainingDataAdvice(string advice) {
    ((ofApp *) ofGetAppPtr())->useTrainingDataAdvice(advice);
}