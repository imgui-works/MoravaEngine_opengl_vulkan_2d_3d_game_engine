/**
 * @package H2M (Hazel to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#include "LayerH2M.h"

#include "Core/Log.h"


namespace H2M
{

	LayerH2M::LayerH2M(const std::string& debugName)
		: m_DebugName(debugName)
	{
		Log::GetLogger()->info("Loading layer '{0}'", m_DebugName);
	}

	LayerH2M::~LayerH2M() {}

}
