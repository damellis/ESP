#include "training-data-manager.cpp"

#include <sstream>

const char kDefaultTrainingDataName[] = "Default";

TrainingDataManager::TrainingDataManager(uint32_t num_classes)
        : num_classes_(num_classes) {
    training_sample_names_.resize(num_classes);
    for (uint32_t i = 0; i < num_classes; i++) {
        default_label_names_.push_back(
            std::string("Label") + std::to_string(i));
    }

    data_.setDatasetName(kDefaultTrainingDataName);
}

bool TrainingDataManager::setNumDimensions(uint32_t dim) {
    return data_.setNumDimensions(dim);
}

bool TrainingDataManager::setDatasetName(const std::string name) {
    return data_.setDatasetName(name);
}

bool TrainingDataManager::setDatasetName(const char* const name) {
    if ((name != NULL) && (name[0] == '\0')) {
        return data_.setDatasetName(std::string(name));
    }
    return false;
}

std::string& TrainingDataManager::getTrainingSampleName(
    uint32_t label, uint32_t index) {
    const auto& name = training_sample_names_[label][index];
    if (name.first) {
        return name.second;
    } else {
        return default_label_names_[label] + " [" + std::to_string(index) + "]";
    }
}

bool TrainingDataManager::setTrainingSampleName(
    uint32_t label, uint32_t index, const std::string new_name) {
    auto& name = training_sample_names_[label][index];
    name.first = true;
    name.second = new_name;
}

bool TrainingDataManager::addSample(uint32_t label, const MatrixFloat& sample) {
    if (label > num_classes_) {
        return false;
    }

    // By default, set the name be <false, ""> so we will use the default name.
    training_sample_names_[label].push_back(
        std::make_pair(false, std::string()));
    data_.addSample(label, sample);
}

bool TrainingDataManager::setNameForLabel(const std::string name, uint32_t label) {
    if (label > num_classes_) {
        return false;
    }

    default_label_names_[label] = name;
}

uint32_t TrainingDataManager::getNumSampleForLabel(uint32_t label) {
    return data_.getClassData(label).getNumSamples();
}

bool TrainingDataManager::deleteSample(uint32_t label, uint32_t index) {
    // The implementation first remove all data and then add them back. This is
    // a temporary solution because GRT::TimeSeriesClassificationData doesn't
    // allow per-sample operation.
    TimeSeriesClassificationData data = data_.getClassData(label);
    data_.eraseAllSamplesWithClassLabel(label);

    for (uint32_t i = 0; i < data.getNumSamples(); i++) {
        if (i != index) {
            training_data_.addSample(label, data[i].getData());
        }
    }

    auto& names = training_sample_names_[label];
    names.erase(names.begin() + index);

    return true;
}

bool TrainingDataManager::deleteSample(uint32_t label, uint32_t index) {
    // The implementation first remove all data and then add them back. This is
    // a temporary solution because GRT::TimeSeriesClassificationData doesn't
    // allow per-sample operation.
    TimeSeriesClassificationData data = data_.getClassData(label);
    data_.eraseAllSamplesWithClassLabel(label);

    for (uint32_t i = 0; i < data.getNumSamples(); i++) {
        if (i != index) {
            data_.addSample(label, data[i].getData());
        }
    }

    auto& names = training_sample_names_[label];
    names.erase(names.begin() + index);
    return true;
}

bool TrainingDataManager::trimSample(
    uint32_t label, uint32_t index, uint32_t start, uint32_t end) {
    TimeSeriesClassificationData data = data_.getClassData(label);
    data_.eraseAllSamplesWithClassLabel(label);

    for (uint32_t i = 0; i < data.getNumSamples(); i++) {
        if (i == index) {
            GRT::MatrixDouble sample = data[i].getData();
            GRT::MatrixDouble new_sample;

            for (int row = start; row < end; row++) {
                new_sample.push_back(sample.getRowVector(row));
            }
            training_data_.addSample(label, new_sample);
        } else {
            data_.addSample(label, data[i].getData());
        }
    }
    return true;
}
