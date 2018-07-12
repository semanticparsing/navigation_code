// dason@trio

#ifndef ROBOT_PUBLIC_BUNDLE_PROCESSER_IF_
#define ROBOT_PUBLIC_BUNDLE_PROCESSER_IF_

#include "Robot/Public/serviceHead.h"
#include <string>

namespace robot{
	class BundleProceserIf{
	public:
		BundleProceserIf(){}
		virtual ~BundleProceserIf(){}
		virtual bool Run(robot::TrioDataBundle* ptr_bundle, std::string& message)=0;
	};
}
#endif // ROBOT_PUBLIC_BUNDLE_PROCESSER_IF_
