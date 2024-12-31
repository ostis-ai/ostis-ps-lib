#include "format_translators_module.hpp"

#include "agent/visualisation-translator-agent/scg_visualisation_translator_agent.hpp"
#include "agent/structure-translator-agent/structure_translator_agent.hpp"
#include "agent/key-elements-orderer-agent/dummy_key_elements_orderer_agent.hpp"

namespace formatTranslators
{
SC_MODULE_REGISTER(FormatTranslatorsModule)
    ->Agent<SCgVisualisationTranslatorAgent>()
    ->Agent<DummyKeyElementsOrdererAgent>()
    ->Agent<StructureTranslatorAgent>();
}  // namespace formatTranslators
