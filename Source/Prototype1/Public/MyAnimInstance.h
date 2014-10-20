

#pragma once

#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_neck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_shoulder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_spine_mid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_spine_base;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_shoulder_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_shoulder_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_elbow_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_elbow_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_wrist_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_wrist_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_hand_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_hand_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_hip_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_hip_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_knee_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_knee_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_ankle_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_ankle_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_foot_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FVector SkelControl_foot_r;

	// Rotation angles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_shoulder_elbow_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_elbow_wrist_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_hip_knee_l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_knee_ankle_l;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_shoulder_elbow_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_elbow_wrist_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_hip_knee_r;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkeletonPlacement)
	FRotator SkelControl_angle_knee_ankle_r;
	
public:

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
