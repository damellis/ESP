/** @file training-data-manager.h
 *  @brief TrainingDataManager class that manages the training data and
 *  abstracts out common operations on the data.
 *
 *  @author Ben Zhang (benzh)
 *  @bug Currently we are not doing bound-checking. Operations over invalid
 *       label/index will cause crashing
 */

#pragma once

#include <tuple>

#include <GRT/GRT.h>

/**
 *  @brief TrainingDataManager class encloses GRT::TimeSeriesClassificationData
 *  and improves upon by adding utility functions that relabel, delete or trim
 *  some training samples.
 *
 *  This class will take the sole mutable ownership of the enclosed data. All
 *  operations over the training data have to gone through this class.
 *
 *  Training data can be viewed as a collection of training samples, where each
 *  sample consists of a label (which class this sample belongs to) and the data
 *  (a time-series data).
 *
 *  A few key augmentation to the underlying TimeSeriesClassificationData:
 *    1. Edit (relabel, delete or trim individual samples).
 *    2. Name individual sample.
 *
 *  Each individual sample is addressable by (label, index) tuple. Label starts
 *  from 1 and index starts from 0.
 */
class TrainingDataManager {
  public:
    // Constructor
    TrainingDataManager(uint32_t num_classes);

    // Set the dimension of the training data
    bool setNumDimensions(uint32_t dim);

    // Set the name of the training data
    bool setDatasetName(const std::string name);

    // Set the name of the training data
    bool setDatasetName(const char* const name);

    GRT::TimeSeriesClassificationData getAllData() {
        return data_;
    }

    // =================================================
    //  Functions that enables per-sample naming
    // =================================================

    /// @brief This will modify the default name for this label, changing it
    /// from "Label X" to `name`.
    bool setNameForLabel(const std::string name, uint32_t label);
    std::string getLabelName(uint32_t label);

    /// @brief Format the sample name.
    /// Default label name is "Label X", and the sample name is "Label X [Y]"
    /// Default name can be changed by `setNameForLabel`.
    std::string getSampleName(uint32_t label, uint32_t index);
    bool setSampleName(uint32_t, uint32_t, const std::string);

    // =================================================
    //  Functions that simplifies editing
    // =================================================

    /// @brief Add new sample. Returns false if the label is larger than
    /// configured number of classes.
    bool addSample(uint32_t label, const GRT::MatrixDouble& sample);

    uint32_t getNumSampleForLabel(uint32_t label);

    /// @brief Get the sample by label and index.
    GRT::MatrixDouble getSample(uint32_t label, uint32_t index);

    /// @brief Remove sample by label and the index.
    bool deleteSample(uint32_t label, uint32_t index);

    /// @brief Remove all samples
    bool deleteAllSamples();

    /// @brief Remove all samples
    bool deleteAllSamplesWithLabel(uint32_t label);

    /// @brief Relabel a sample from `label` to `new_label`.
    bool relabelSample(uint32_t label, uint32_t index, uint32_t new_label);

    /// @brief Trim sample. What's left will be [start, end], closed interval.
    bool trimSample(uint32_t label, uint32_t index, uint32_t start, uint32_t end);

    // =================================================
    //  Functions for saving/loading training data
    // =================================================

    inline bool save(const std::string& filename) {
        return data_.save(filename);
    }

    inline bool load(const std::string& filename) {
        return data_.load(filename);
    }

  private:
    uint32_t num_classes_;

    // Name simulates Option<std::string> type. If `Name.first` is true, then
    // the name is valid; else use the default name.
    using Name = std::pair<bool, std::string>;
    std::vector<std::vector<Name>> training_sample_names_;
    std::vector<std::string> default_label_names_;

    // The underlying data store backed up by GRT's TimeSeriesClassificationData
    GRT::TimeSeriesClassificationData data_;

    // Disallow copy and assign
    TrainingDataManager(TrainingDataManager&) = delete;
    void operator=(TrainingDataManager) = delete;
};
