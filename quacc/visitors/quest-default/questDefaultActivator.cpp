#include "OptionsProvider.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "mps/QuestDefaultVisitor.hpp"

using namespace cppmicroservices;

class US_ABI_LOCAL QuestDefaultActivator : public BundleActivator {
public:
  QuestDefaultActivator() {}

  void Start(BundleContext context) {
    auto vis = std::make_shared<quacc::QuestDefaultVisitor>();
    context.RegisterService<quacc::xQuaccVisitor>(vis);
    context.RegisterService<xacc::OptionsProvider>(vis);
  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(QuestDefaultActivator)
