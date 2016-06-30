#include "training-data-manager.h"
#include "gtest/gtest.h"

static const uint32_t kNumClasses = 3;
static const uint32_t kSampleDim = 3;

class TrainingDataManagerTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
        manager = unique_ptr<TrainingDataManager>(
            new TrainingDataManager(kNumClasses));
        manager->setNumDimensions(kSampleDim);
    }

    unique_ptr<TrainingDataManager> manager;
};

TEST_F(TrainingDataManagerTest, TestAddAndRemoveSample) {
    GRT::MatrixDouble sample(1, kSampleDim);
    // Modify the data a bit so that we can be sure the remove works.
    sample[0][0] = 1;
    manager->addSample(0, sample);
    ASSERT_EQ(1, manager->getSample(0, 0)[0][0]);

    sample[0][0] = 2;
    manager->addSample(0, sample);
    ASSERT_EQ(2, manager->getSample(0, 1)[0][0]);

    sample[0][0] = 3;
    manager->addSample(0, sample);
    ASSERT_EQ(3, manager->getSample(0, 2)[0][0]);

    // Check the dimension matching
    ASSERT_EQ(3, manager->getNumSampleForLabel(0));
    ASSERT_EQ(0, manager->getNumSampleForLabel(1));
    ASSERT_EQ(0, manager->getNumSampleForLabel(2));

    // Delete label 1 index 1 (the middle sample)
    manager->deleteSample(0, 1);
    ASSERT_EQ(2, manager->getNumSampleForLabel(0));
    ASSERT_EQ(3, manager->getSample(0, 1)[0][0]);
}
