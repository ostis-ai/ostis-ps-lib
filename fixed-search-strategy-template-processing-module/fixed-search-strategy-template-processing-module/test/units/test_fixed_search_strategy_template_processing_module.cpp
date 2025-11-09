#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <keynodes/keynodes.hpp>
#include <data/parameterized_template_builder.hpp>

std::string const TEST_FILES_DIR_PATH =
    "/Users/nikitazotov/Development/apps/ostis/ostis-ai/ostis-ps-lib/fixed-search-strategy-template-processing-module/"
    "fixed-search-strategy-template-processing-module/test/units/../test-structures/";

using FixedSearchStrategyTemplateProcessingModuleTest = ScMemoryTest;

TEST_F(FixedSearchStrategyTemplateProcessingModuleTest, SearchByFixedSearchStrategyTemplate)
{
  ScAgentContext context;
  ScsLoader loader;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "fixed_search_strategy_template.scs");

  ScIterator3Ptr it3 =
      context.CreateIterator3(Keynodes::nrel_fixed_search_strategy_template, ScType::ConstPosArc, ScType::ConstNode);
  ASSERT_TRUE(it3->Next());
  ScAddr const templateAddr = it3->Get(2);

  ScAddr const & bsuirAddr = context.SearchElementBySystemIdentifier("BSUIR");
  ASSERT_TRUE(bsuirAddr.IsValid());
  ScAddr const & conceptUniversityAddr = context.SearchElementBySystemIdentifier("concept_university");
  ASSERT_TRUE(conceptUniversityAddr.IsValid());
  it3 = context.CreateIterator3(conceptUniversityAddr, ScType::ConstPosArc, bsuirAddr);
  ASSERT_TRUE(it3->Next());
  ScAddr const & arcToBsuirAddr = it3->Get(1);

  auto logger = utils::ScLogger(utils::ScLogger::ScLogType::Console, "", utils::ScLogLevel::Debug, true);
  auto const & templ = ParameterizedTemplateBuilder::BuildTemplate(context, logger, templateAddr);

  TemplateArguments arguments{context, logger};
  arguments.Add(conceptUniversityAddr, arcToBsuirAddr, bsuirAddr);

  TemplateResults results;
  EXPECT_TRUE(templ->Apply(arguments, results));

  results.ForEach(
      [](TemplateResult const & result)
      {
        std::cout << "Result 0 : " << result.GetIndex() << std::endl;
        result.ForEach(
            [](TemplateResult const & result)
            {
              std::cout << "Result 1 : " << result.GetIndex() << std::endl;
              result.ForEach(
                  [](TemplateResult const & result)
                  {
                    std::cout << "Result 2 : " << result.GetIndex() << std::endl;
                  });
            });
      });
}
