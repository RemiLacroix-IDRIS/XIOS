#include "filter.hpp"

namespace xios
{
  CFilter::CFilter(CGarbageCollector& gc, size_t inputSlotsCount, IFilterEngine* engine)
    : CInputPin(gc, inputSlotsCount)
    , COutputPin()
    , engine(engine)
    , inputSlotCount(inputSlotCount)
  { /* Nothing to do */ }

  void CFilter::onInputReady(std::vector<CDataPacketPtr> data)
  {
    CDataPacketPtr outputPacket = engine->apply(data);
    if (outputPacket)
    {
      printf("filter/filter.cpp : deliverOuput(outputPacket)\n");
      deliverOuput(outputPacket);
      printf("filter/filter.cpp : deliverOuput(outputPacket) OKOK\n");
    }
  }
} // namespace xios
