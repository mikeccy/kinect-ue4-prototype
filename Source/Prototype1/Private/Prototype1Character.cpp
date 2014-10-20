// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype1.h"
#include "Prototype1Character.h"

#include "MyAnimInstance.h"

#include "AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include <Kinect.h>
#include "HideWindowsPlatformTypes.h"

//////////////////////////////////////////////////////////////////////////
// APrototype1Character

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != nullptr){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = nullptr;
	}
}

inline FVector Transform(CameraSpacePoint& point, FMatrix* rotM, FMatrix* transM, FMatrix* scaleM)
{
	//UE_LOG(LogClass, Log, _T("INFO rotM111 %f, %f, %f"), rotM->M[0][0], rotM->M[0][1], rotM->M[0][2]);
	//UE_LOG(LogClass, Log, _T("INFO rotM222 %f, %f, %f"), rotM->M[1][0], rotM->M[1][1], rotM->M[1][2]);
	//UE_LOG(LogClass, Log, _T("INFO rotM333 %f, %f, %f"), rotM->M[2][0], rotM->M[2][1], rotM->M[2][2]);
	//FVector4 vec = scaleM->TransformFVector4(transM->TransformFVector4(rotM->TransformFVector4(FVector(point.X, point.Y, point.Z))));
	//FVector4 vec = scaleM->TransformFVector4(transM->TransformFVector4(FVector(point.X, point.Y, point.Z)));
	
	//FVector4 vec = rotM->TransformFVector4(FVector(point.X, -1 * point.Z, point.Y));
	//return FVector(vec.X, vec.Y, vec.Z);
	float multiplier = 100;
	return FVector(point.X * multiplier, -1 * point.Z * multiplier, point.Y * multiplier);
}

APrototype1Character::APrototype1Character(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	CharacterMovement->JumpZVelocity = 600.f;
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUseControllerViewRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	// Enable tick for Kinect data capture
	PrimaryActorTick.bCanEverTick = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void APrototype1Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up Kinect devices
	// TODO expose as a service entry point
	InitKinect();
	
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAxis("MoveForward", this, &APrototype1Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APrototype1Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APrototype1Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APrototype1Character::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APrototype1Character::TouchStarted);
}


void APrototype1Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void APrototype1Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APrototype1Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APrototype1Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APrototype1Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

int APrototype1Character::InitKinect()
{
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor(&pSensor);
	if (FAILED(hResult)){
		TCHAR* pMessage = _T("GetDefaultKinectSensor"); UE_LOG(LogClass, Log, _T("ERROR %s"), pMessage);
		return -1;
	}
	hResult = pSensor->Open();
	if (FAILED(hResult)){
		TCHAR* pMessage = _T("pSensor->Open"); UE_LOG(LogClass, Log, _T("ERROR %s"), pMessage);
		return -1;
	}

	hResult = pSensor->get_BodyFrameSource(&pBodySource);
	if (FAILED(hResult)){
		TCHAR* pMessage = _T("pSensor->get_BodyFrameSource"); UE_LOG(LogClass, Log, _T("ERROR %s"), pMessage);
		return -1;
	}

	hResult = pBodySource->OpenReader(&pBodyReader);
	if (FAILED(hResult)){
		TCHAR* pMessage = _T("pBodySource->OpenReader"); UE_LOG(LogClass, Log, _T("ERROR %s"), pMessage);
		return -1;
	}

	hResult = pSensor->get_CoordinateMapper(&pCoordinateMapper);
	if (FAILED(hResult)){
		TCHAR* pMessage = _T("pSensor->get_CoordinateMapper"); UE_LOG(LogClass, Log, _T("ERROR %s"), pMessage);
		return -1;
	}

	return 0;
}

void APrototype1Character::Tick(float DeltaSeconds)
{
	if (tickSkipped < TICK_TO_SKIP) 
	{
		needUpdating = false;
		tickSkipped += 1;
		//UE_LOG(LogClass, Log, _T("INFO sleep"));
	}
	else
	{
		needUpdating = true;
		tickSkipped = 0;
		//UE_LOG(LogClass, Log, _T("INFO update"));
	}

	HRESULT hResult = S_OK;
	IBodyFrame* pBodyFrame = nullptr;
	hResult = pBodyReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hResult))
	{
		//TODO body index selectable

		IBody* pBody[BODY_COUNT] = { 0 };
		hResult = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);
		if (SUCCEEDED(hResult))
		{
			if (needUpdating)
			{
				// Retrieve the floor plane
				// rotate around x axis for now, based on the assumption that kinect is placed horizontally
				// Kinect coordinate space: http://msdn.microsoft.com/en-us/library/hh973078.aspx
				Vector4* pFloorClipPlane = new Vector4();
				hResult = pBodyFrame->get_FloorClipPlane(pFloorClipPlane);
				
				if (SUCCEEDED(hResult))
				{
					// calculate the rotation and translation matrix here for the coordinates to use
					// translate floor plain to XZ zero plane
					double sinSign = pFloorClipPlane->z > 0 ? -1 : 1;
					double cosSign = pFloorClipPlane->y > 0 ? -1 : 1;
					double distance = sqrt(pow((double)pFloorClipPlane->y, 2) + pow((double)pFloorClipPlane->z, 2));
					double sin = sinSign * (double)pFloorClipPlane->z / distance;
					double cos = cosSign * (double)pFloorClipPlane->y / distance;

					rotM->M[0][0] = 1; rotM->M[0][1] = 0; rotM->M[0][2] = 0; rotM->M[0][3] = 0;
					rotM->M[1][0] = 0; rotM->M[1][1] = cos; rotM->M[1][2] = -1 * sin; rotM->M[1][3] = 0;
					rotM->M[2][0] = 0; rotM->M[2][1] = sin; rotM->M[2][2] = cos; rotM->M[2][3] = 0;
					rotM->M[3][0] = 0; rotM->M[3][1] = 0; rotM->M[3][2] = 0; rotM->M[3][3] = 1;

					transM->M[0][0] = 1; transM->M[0][1] = 0; transM->M[0][2] = 0; transM->M[0][3] = 0;
					transM->M[1][0] = 0; transM->M[1][1] = 1; transM->M[1][2] = 0; transM->M[1][3] = 0;
					transM->M[2][0] = 0; transM->M[2][1] = 0; transM->M[2][2] = 1; transM->M[2][3] = 0;
					transM->M[3][0] = 0; transM->M[3][1] = 0; transM->M[3][2] = 0; transM->M[3][3] = 1;
				}

				if (pFloorClipPlane != nullptr) {
					delete pFloorClipPlane;
					pFloorClipPlane = nullptr;
				}
			}

			if (Mesh)
			{
				UMyAnimInstance* Animation = Cast<UMyAnimInstance>(Mesh->GetAnimInstance());
				if (Animation)
				{
					for (int count = 0; count < BODY_COUNT; count++)
					{
						BOOLEAN bTracked = false;
						hResult = pBody[count]->get_IsTracked(&bTracked);
						if (SUCCEEDED(hResult) && bTracked)
						{
							Joint joint[JointType::JointType_Count];
							hResult = pBody[count]->GetJoints(JointType::JointType_Count, joint);
							if (SUCCEEDED(hResult))
							{
								for (int type = 0; type < JointType::JointType_Count; type++)
								{
									switch (type)
									{
									case JointType_SpineBase:
										Animation->SkelControl_spine_base = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_SpineMid:
										Animation->SkelControl_spine_mid = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_Neck:
										Animation->SkelControl_neck = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_Head:
										Animation->SkelControl_head = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_ShoulderLeft:
										Animation->SkelControl_shoulder_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_ElbowLeft:
										Animation->SkelControl_elbow_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_WristLeft:
										Animation->SkelControl_wrist_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_HandLeft:
										Animation->SkelControl_hand_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_ShoulderRight:
										Animation->SkelControl_shoulder_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_ElbowRight:
										Animation->SkelControl_elbow_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_WristRight:
										Animation->SkelControl_wrist_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_HandRight:
										Animation->SkelControl_hand_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_HipLeft:
										Animation->SkelControl_hip_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_KneeLeft:
										Animation->SkelControl_knee_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_AnkleLeft:
										Animation->SkelControl_ankle_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_FootLeft:
										Animation->SkelControl_foot_l = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_HipRight:
										Animation->SkelControl_hip_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_KneeRight:
										Animation->SkelControl_knee_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_AnkleRight:
										Animation->SkelControl_ankle_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_FootRight:
										Animation->SkelControl_foot_r = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_SpineShoulder:
										Animation->SkelControl_shoulder = Transform(joint[type].Position, rotM, transM, scaleM);
										break;
									case JointType_HandTipLeft:
									case JointType_ThumbLeft:
									case JointType_HandTipRight:
									case JointType_ThumbRight:
									default:
										break;
									}
								}

								//calcaulate rotation here

								Animation->SkelControl_angle_shoulder_elbow_l = (Animation->SkelControl_elbow_l - Animation->SkelControl_shoulder_l).Rotation();
								Animation->SkelControl_angle_elbow_wrist_l = (Animation->SkelControl_wrist_l - Animation->SkelControl_elbow_l).Rotation();
								Animation->SkelControl_angle_hip_knee_l = (Animation->SkelControl_knee_l - Animation->SkelControl_hip_l).Rotation();
								Animation->SkelControl_angle_knee_ankle_l = (Animation->SkelControl_ankle_l - Animation->SkelControl_knee_l).Rotation();

								Animation->SkelControl_angle_shoulder_elbow_r = (Animation->SkelControl_elbow_r - Animation->SkelControl_shoulder_r).Rotation();
								Animation->SkelControl_angle_elbow_wrist_r = (Animation->SkelControl_wrist_r - Animation->SkelControl_elbow_r).Rotation();
								Animation->SkelControl_angle_hip_knee_r = (Animation->SkelControl_knee_r - Animation->SkelControl_hip_r).Rotation();
								Animation->SkelControl_angle_knee_ankle_r = (Animation->SkelControl_ankle_r - Animation->SkelControl_knee_r).Rotation();

							}
						}
					}
				}
			}
		}
		for (int count = 0; count < BODY_COUNT; count++)
		{
			SafeRelease(pBody[count]);
		}
	}
	SafeRelease(pBodyFrame);
}

void APrototype1Character::Destroyed()
{
	SafeRelease(pBodySource);
	SafeRelease(pBodyReader);
	SafeRelease(pCoordinateMapper);

	if (pSensor){
		pSensor->Close();

	}
	SafeRelease(pSensor);

	delete rotM;
	delete transM;
	delete scaleM;
}

// some dusty code...

//ColorSpacePoint colorSpacePoint = { 0 };
//pCoordinateMapper->MapCameraPointToColorSpace(joint[type].Position, &colorSpacePoint);
//int x = static_cast<int>(colorSpacePoint.X);
//int y = static_cast<int>(colorSpacePoint.Y);
//TCHAR* pMessage = _T("yo"); UE_LOG(LogClass, Log, _T("INFO %s"), pMessage);
//UE_LOG(LogClass, Log, _T("INFO x,y %d, %d"), x, y);
//UE_LOG(LogClass, Log, _T("INFO x,y,z %f, %f, %f"), Animation->SkelControl_hand_r.X, Animation->SkelControl_hand_r.Y, Animation->SkelControl_hand_r.Z);

/*
if (Mesh) {
FVector vec0 = Mesh->GetBoneLocation("head", 0);
FVector vec1 = Mesh->GetBoneLocation("spine_01", 0);
FVector vec2 = Mesh->GetBoneLocation("thigh_twist_01_l", 0);
FVector vec3 = Mesh->GetBoneLocation("thigh_l", 0);

//Mesh->GetBoneMatrix()

UE_LOG(LogClass, Log, _T("INFO ue4 head=x,y,z %f, %f, %f"), vec0.X, vec0.Y, vec0.Z);
UE_LOG(LogClass, Log, _T("INFO ue4 spine_01=x,y,z %f, %f, %f"), vec1.X, vec1.Y, vec1.Z);
UE_LOG(LogClass, Log, _T("INFO ue4 thigh_twist_01_l=x,y,z %f, %f, %f"), vec2.X, vec2.Y, vec2.Z);
UE_LOG(LogClass, Log, _T("INFO ue4 thigh_l=x,y,z %f, %f, %f"), vec3.X, vec3.Y, vec3.Z);
}
*/
/*
//No Mesh?
UE_LOG(LogClass, Log, _T("INFO checking mesh"));
if (!Mesh) return;

UMyAnimInstance * Animation = Cast<UMyAnimInstance>(Mesh->GetAnimInstance());

//No Anim Instance Acquired?
UE_LOG(LogClass, Log, _T("INFO checking animation"));
//TCHAR* pMessage = _T("checking animation"); UE_LOG(LogClass, Log, _T("INFO %s"), pMessage);
if (!Animation) return;

Animation->SkelControl_LeftLowerLegPos = FVector(0, 0, 0);
Animation->SkelControl_LeftUpperLegPos = FVector(0, 0, 0);
Animation->SkelControl_LeftFootRotation = FRotator(10, 20, 0);


UE_LOG(LogClass, Log, _T("INFO end animation"));
//TCHAR* pMessage = _T("end animation"); UE_LOG(LogClass, Log, _T("INFO %s"), pMessage);
*/