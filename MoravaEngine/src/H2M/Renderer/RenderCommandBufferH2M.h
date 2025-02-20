/**
 * @package H2M (Hazel to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#pragma once

#include "H2M/Core/RefH2M.h"
#include "H2M/Renderer/PipelineH2M.h"

#include <string>


namespace H2M
{

	class RenderCommandBufferH2M : public RefCountedH2M
	{
	public:
		virtual ~RenderCommandBufferH2M() {}

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual float GetExecutionGPUTime(uint32_t frameIndex, uint32_t queryIndex = 0) const = 0;
		virtual const PipelineStatisticsH2M& GetPipelineStatistics(uint32_t frameIndex) const = 0;

		virtual uint64_t BeginTimestampQuery() = 0;
		virtual void EndTimestampQuery(uint64_t queryID) = 0;

		static RefH2M<RenderCommandBufferH2M> Create(uint32_t count = 0, const std::string& debugName = "");
		static RefH2M<RenderCommandBufferH2M> CreateFromSwapChain(const std::string& debugName = "");

	};

}
