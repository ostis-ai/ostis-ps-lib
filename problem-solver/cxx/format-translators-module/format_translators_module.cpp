#include "format_translators_module.hpp"

#include "agent/visualisation-translator-agent/scg_visualisation_translator_agent.hpp"

namespace formatTranslators
{
SC_MODULE_REGISTER(FormatTranslatorsModule)->Agent<SCgVisualisationTranslatorAgent>();
}  // namespace formatTranslators
