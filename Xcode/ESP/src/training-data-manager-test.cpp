#include "training-data-manager.h"
#include "gtest/gtest.h"

static const uint32_t kNumClasses = 3;
static const uint32_t kSampleDim = 3;

class TrainingDataManagerTest : public ::testing::Test {
  protected:
    // Properly populate the manager for testing
    virtual void SetUp() {
        manager = unique_ptr<TrainingDataManager>(
            new TrainingDataManager(kNumClasses));
        manager->setNumDimensions(kSampleDim);

        GRT::MatrixDouble sample(1, kSampleDim);
        sample[0][0] = 1;
        manager->addSample(1, sample);

        sample[0][0] = 2;
        manager->addSample(1, sample);

        sample[0][0] = 3;
        manager->addSample(1, sample);
    }

    unique_ptr<TrainingDataManager> manager;
};

TEST_F(TrainingDataManagerTest, BasicAddAndGet) {
    // Check the populated data (see TrainingDataManagerTest::SetUp()).
    ASSERT_EQ(1, manager->getSample(1, 0)[0][0]);
    ASSERT_EQ(2, manager->getSample(1, 1)[0][0]);
    ASSERT_EQ(3, manager->getSample(1, 2)[0][0]);

    // Check the length
    ASSERT_EQ(3, manager->getNumSampleForLabel(1));
    ASSERT_EQ(0, manager->getNumSampleForLabel(2));
    ASSERT_EQ(0, manager->getNumSampleForLabel(3));
}

TEST_F(TrainingDataManagerTest, TestDeleteSample) {
    // Delete label 1 index 1 (the middle sample)
    // We are expecting the following change
    // [1, 0, 0], [2, 0, 0], [3, 0, 0]
    // [1, 0, 0],          , [3, 0, 0]
    manager->deleteSample(1, 1);
    ASSERT_EQ(2, manager->getNumSampleForLabel(1));
    ASSERT_EQ(3, manager->getSample(1, 1)[0][0]);
}

TEST_F(TrainingDataManagerTest, TestTrimSample) {
    uint32_t num_point = 10;
    GRT::MatrixDouble sample(num_point, kSampleDim);
    for (uint32_t i = 0; i < num_point; i++) {
        sample[i][0] = i;
    }
    manager->addSample(1, sample);
    ASSERT_EQ(4, manager->getNumSampleForLabel(1));

    // Till here, we have four samples. After trimming (from 2 to 4), sample 4
    // (index 3) is expected to have the following change:
    // [0, 1, 2, ..., 10] -> [2, 3, 4]
    manager->trimSample(1, 3, 2, 4);
    ASSERT_EQ(4, manager->getNumSampleForLabel(1));

    GRT::MatrixDouble new_sample = manager->getSample(1, 3);
    ASSERT_EQ(3, new_sample.getNumRows());
    ASSERT_EQ(2, new_sample[0][0]);
    ASSERT_EQ(3, new_sample[1][0]);
    ASSERT_EQ(4, new_sample[2][0]);
}

TEST_F(TrainingDataManagerTest, TestName) {
    // Default name
    ASSERT_STREQ("Label 0 [1]", manager->getSampleName(1, 1).c_str());
    manager->setNameForLabel("MyLabel", 1);
    ASSERT_STREQ("MyLabel [1]", manager->getSampleName(1, 1).c_str());
}
