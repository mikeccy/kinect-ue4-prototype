

#include "Prototype1.h"
#include "MyAnimInstance.h"


UMyAnimInstance::UMyAnimInstance(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}


