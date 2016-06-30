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
        manager->addSample(0, sample);

        sample[0][0] = 2;
        manager->addSample(0, sample);

        sample[0][0] = 3;
        manager->addSample(0, sample);
    }

    unique_ptr<TrainingDataManager> manager;
};

TEST_F(TrainingDataManagerTest, BasicAddAndGet) {
    // Check the populated data (see TrainingDataManagerTest::SetUp()).
    ASSERT_EQ(1, manager->getSample(0, 0)[0][0]);
    ASSERT_EQ(2, manager->getSample(0, 1)[0][0]);
    ASSERT_EQ(3, manager->getSample(0, 2)[0][0]);

    // Check the length
    ASSERT_EQ(3, manager->getNumSampleForLabel(0));
    ASSERT_EQ(0, manager->getNumSampleForLabel(1));
    ASSERT_EQ(0, manager->getNumSampleForLabel(2));
}

TEST_F(TrainingDataManagerTest, TestDeleteSample) {
    // Delete label 1 index 1 (the middle sample)
    manager->deleteSample(0, 1);
    ASSERT_EQ(2, manager->getNumSampleForLabel(0));
    ASSERT_EQ(3, manager->getSample(0, 1)[0][0]);
}

TEST_F(TrainingDataManagerTest, TestName) {
    // Default name
    ASSERT_STREQ("Label 0 [1]", manager->getSampleName(0, 1).c_str());
    manager->setNameForLabel("MyLabel", 0);
    ASSERT_STREQ("MyLabel [1]", manager->getSampleName(0, 1).c_str());
}
