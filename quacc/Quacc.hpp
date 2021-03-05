/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * Copyright (c) 2021, Milos Prokop
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - Mengsu Chen 2017.7
 *   Modifications to include the Quacc - Milos Prokop 2021.2
 *
 **********************************************************************************/

#ifndef QUACC_HPP_
#define QUACC_HPP_

#include "xacc.hpp"
#include "xacc_service.hpp"
#include <cassert>

#include "QuEST.h"
#include "visitors/QuaccVisitor.hpp"

namespace quacc {

	class Quacc : public Accelerator {
	public:

	  void initialize(const xacc::HeterogeneousMap &params = {}) override {
		if (xacc::optionExists("quacc-verbose")) {
		  __verbose = 1;
		} else {
		  __verbose = 0;
		}

		// Clear the cached configs on XaccQuest initialize.
		options.clear();
		// Force a configuration update,
		// which will update the cache appropriately.
		updateConfiguration(params);
	  }

	  // This is called post-initialize to add/update configurations.
	  void updateConfiguration(const HeterogeneousMap &config) override {

		if (config.keyExists<int>("nbQbits")){

		  env = createQuESTEnv();
		  qreg = createQureg(config.get<int>("nbQbits"), env);

		  const QuESTEnv* env_address = static_cast<const QuESTEnv*>(&env);
		  const Qureg* qreg_address = static_cast<const Qureg*>(&qreg);

		  std::stringstream ss_env_ptr;
		  std::stringstream ss_qreg_ptr;

		  ss_env_ptr << env_address;
		  ss_qreg_ptr << qreg_address;

		  xacc::setOption("use_global_qreg", "true");
		  xacc::setOption("global_env", ss_env_ptr.str());
		  xacc::setOption("global_qreg", ss_qreg_ptr.str());

		}

		if (config.keyExists<bool>("verbose") && config.get<bool>("verbose")) {
		  __verbose = 1;
		}
		if (config.keyExists<bool>("vqe-mode")) {
		  vqeMode = config.get<bool>("vqe-mode");
		}

		if (config.stringExists("quacc-visitor") ||
			config.stringExists("backend")) {
		  // Get the specific QUACC visitor, either using the `tnqvm-visitor` key
		  // or the `backend` key.
		  const auto requestedBackend = config.stringExists("tnqvm-visitor")
											? config.getString("tnqvm-visitor")
											: config.getString("backend");
		  const auto &allVisitorServices = xacc::getServices<xQuaccVisitor>();
		  // We must have at least one XaccQuest service registered.
		  assert(!allVisitorServices.empty());
		  bool foundRequestedBackend = false;

		  for (const auto& registeredService: allVisitorServices)
		  {
			if (registeredService->name() == requestedBackend)
			{
			  // Found it, use that service name.
			  backendName = registeredService->name();
			  foundRequestedBackend = true;
			  break;
			}
		  }
		  // A visitor backend was explicitly specified but the corresponding service cannot be found,
		  // e.g. the service name was misspelled.
		  if (!foundRequestedBackend)
		  {
			backendName = DEFAULT_VISITOR_BACKEND;
			xacc::warning("The requested XaccQuest visitor backend '" + requestedBackend + "' cannot be found in the service registry. Please make sure the name is correct and the service is installed.\n"
			  "The default visitor backend of type '" + DEFAULT_VISITOR_BACKEND + "' will be used.");
		  }
		}

		if (config.keyExists<int>("shots")) {
		  nbShots = config.get<int>("shots");
		  if (nbShots < 1) {
			xacc::error("Invalid 'shots' parameter.");
		  }

		}

		// Updated the cached configurations (to be sent on to visitor)
		// Note: Accelerator-level configs (visitor name, shots, vqe mode, etc.)
		// have been handled here, i.e. retrieving from the new config map. The rest
		// of the configs are visitor-specific and will be forwarded to them
		// accordingly.
		options.merge(config);
	  }
	  const std::vector<std::string> configurationKeys() override { return {}; }
	//   const std::string getSignature() override {return name()+":";}

	  void
	  execute(std::shared_ptr<AcceleratorBuffer> buffer,
			  const std::shared_ptr<xacc::CompositeInstruction> kernel) override;

	  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				   const std::vector<std::shared_ptr<CompositeInstruction>>
					   functions) override;

	  const std::string name() const override { return "quest"; }

	  const std::string description() const override {
		return "XACC QuEST Accelerator";
	  }

	  const std::string& getVisitorName() const { return backendName; }

	  ~Quacc() {

		  if(xacc::optionExists("use_global_qreg") && xacc::getOption("use_global_qreg") == "true"){

			  std::stringstream qreg_adress(xacc::getOption("global_qreg"));
			  std::stringstream env_adress(xacc::getOption("global_env"));
			  Qureg *qregPtr;
			  QuESTEnv *envPtr;

			  void *tempPointer1, *tempPointer2;
			  qreg_adress >> tempPointer1;
			  env_adress >> tempPointer2;

			  Qureg* qreg_final = (Qureg*)tempPointer1;
			  QuESTEnv* env_final = (QuESTEnv*)tempPointer2;

			  destroyQureg(*qreg_final, *env_final);
			  destroyQuESTEnv(*env_final);

		  }

	  }

	  virtual HeterogeneousMap getExecutionInfo() const override {
		auto result = visitor->getExecutionInfo();
		result.insert("visitor", visitor->name());
		return result;
	  }

	  int verbose() const { return __verbose; }
	  void verbose(int level) { __verbose = level; }
	  void set_verbose(int level) { __verbose = level; }
	  void mute() { __verbose = 0; }
	  void unmute() { __verbose = 1; } // default to 1

	protected:
	  std::shared_ptr<xQuaccVisitor> visitor;

	private:

	  QuESTEnv env;
	  Qureg qreg;

	  int __verbose = 1;
	  bool executedOnce = false;
	  bool vqeMode = true;

	  static const std::string DEFAULT_VISITOR_BACKEND;
	  // The backend name that is configured.
	  // Initialized to the default.
	  std::string backendName = DEFAULT_VISITOR_BACKEND;
	  // Number of *shots* (randomized runs) requested.
	  // If not specified (i.e. left as -1),
	  // then we don't return the binary measurement result (as a bit string).
	  // This is to make sure that on the XACC side, it can interpret the avarage-Z result correctly.
	  int nbShots = -1;
	  // Cache of the QUACC options (to send on to the visitor)
	  HeterogeneousMap options;

	};
} // namespace quacc

#endif //XACC_QUEST
