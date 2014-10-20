// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "AllowWindowsPlatformTypes.h"
#include <Kinect.h>
#include "HideWindowsPlatformTypes.h"

#include "Prototype1Character.generated.h"

UCLASS(config=Game)
class APrototype1Character : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class UCameraComponent> FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handles changes from Kinect body frame */
	virtual void Tick(float DeltaSeconds) override;

	/** Clean up work when destroy */
	virtual void Destroyed() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

private:
	int InitKinect();

	// Kinect Raw Output
	IKinectSensor* pSensor;
	IBodyFrameSource* pBodySource;
	IBodyFrameReader* pBodyReader;
	ICoordinateMapper* pCoordinateMapper;

	// for calculations that do not need rapid update
	const int TICK_TO_SKIP = 60;
	bool needUpdating = false;
	int tickSkipped = 60;

	// kinect to unreal space tranformation
	// rotate then translate
	// not updated on every tick
	FMatrix* rotM = new FMatrix();
	FMatrix* transM = new FMatrix(); //?? needed

	// unreal unit to kinect, 1UU = 1cm so 100
	FMatrix* scaleM = new FMatrix(
		FPlane(100, 0, 0, 0),
		FPlane(0, 100, 0, 0),
		FPlane(0, 0, 100, 0),
		FPlane(0, 0, 0, 1));

	// environment input
	// mirror location etc.

};

