#include <gtest/gtest.h>

#include "asmlgen/application/dataset/dataset_entry.h"

namespace dataset::tests
{

TEST(Sequence_Tests, SequenceId_Next_IdSequenceIncrementedOnInstance)
{
  DatasetEntry entry_1 {};
  DatasetEntry entry_2 {};
  DatasetEntry entry_3 {};

  ASSERT_EQ(entry_1.GetId(), 0);
  ASSERT_EQ(entry_2.GetId(), 1);
  ASSERT_EQ(entry_3.GetId(), 2);
}

} // namespace dataset::tests