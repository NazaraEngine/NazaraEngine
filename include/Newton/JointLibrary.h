/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef NEWTON_CUSTOM_JOINTS_H_INCLUDED_
#define NEWTON_CUSTOM_JOINTS_H_INCLUDED_


#include "Newton.h"
//#include "CustomJointLibraryStdAfx.h"

#ifdef _NEWTON_STATIC_LIB
	#define JOINTLIBRARY_API
#else
	#ifdef JOINTLIBRARY_LIB
		#define JOINTLIBRARY_API 
	#else
		#ifdef JOINTLIBRARY_EXPORTS
			#define JOINTLIBRARY_API __declspec(dllexport)
		#else
			#define JOINTLIBRARY_API __declspec(dllimport)
		#endif
	#endif
#endif




#ifdef __cplusplus 
extern "C" {
#endif

	// Generic joint call back and user data
	typedef struct NewtonUserJoint{} NewtonUserJoint;
	typedef void (*NewtonUserJointDestructorCallback) (const NewtonUserJoint* me);
	typedef void (*NewtonUserJointSubmitConstraintCallback) (const NewtonUserJoint* me, dFloat timestep, int threadIndex);


	// generic joint functions
	JOINTLIBRARY_API void CustomDestroyJoint(const NewtonUserJoint *joint);
	JOINTLIBRARY_API NewtonJoint* CustomGetNewtonJoint (const NewtonUserJoint *joint);
	JOINTLIBRARY_API int CustomGetJointID (const NewtonUserJoint *joint);
	JOINTLIBRARY_API void CustomSetJointID (const NewtonUserJoint *joint, int rttI);
	JOINTLIBRARY_API const NewtonBody* CustomGetBody0 (const NewtonUserJoint *joint);
	JOINTLIBRARY_API const NewtonBody* CustomGetBody1 (const NewtonUserJoint *joint);
	JOINTLIBRARY_API int CustomGetBodiesCollisionState (const NewtonUserJoint *joint);
	JOINTLIBRARY_API void CustomSetBodiesCollisionState (const NewtonUserJoint *joint, int state);
	JOINTLIBRARY_API void* CustomGetUserData (const NewtonUserJoint *joint);
	JOINTLIBRARY_API void CustomSetUserData (const NewtonUserJoint *joint, void* userData);
	JOINTLIBRARY_API void CustomSetDestructorCallback (const NewtonUserJoint *joint, NewtonUserJointDestructorCallback callback);
	JOINTLIBRARY_API void CustomSetSubmitContraintCallback (const NewtonUserJoint *joint, NewtonUserJointSubmitConstraintCallback callback);


	// this is a plain blank joint that can be used by advanced users who want to make their own joints
	// but that can only use languages that can only interface with C code. 
	// we recommend using the CPP library to make the joints and then add a C interface, but this join is here for completion
	typedef void (*BlankJointGetInfo) (const NewtonUserJoint* me, NewtonJointRecord* info);
	JOINTLIBRARY_API NewtonUserJoint* CustomCreateBlankJoint(int maxDof, const NewtonBody* body0, const NewtonBody* body1, BlankJointGetInfo info);

	// Kinematic control joint
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomKinematicController (const NewtonBody* targetBody, dFloat* attachmentPointInGlobalSpace);
	JOINTLIBRARY_API void CustomKinematicControllerSetPickMode (const NewtonUserJoint *pick, int mode);
	JOINTLIBRARY_API void CustomKinematicControllerSetMaxLinearFriction(const NewtonUserJoint *pick, dFloat accel); 
	JOINTLIBRARY_API void CustomKinematicControllerSetMaxAngularFriction(const NewtonUserJoint *pick, dFloat alpha); 
	JOINTLIBRARY_API void CustomKinematicControllerSetTargetPosit (const NewtonUserJoint *pick, dFloat* posit); 
	JOINTLIBRARY_API void CustomKinematicControllerSetTargetRotation (const NewtonUserJoint *pick, dFloat* rotation); 
	JOINTLIBRARY_API void CustomKinematicControllerSetTargetMatrix (const NewtonUserJoint *pick, dFloat* matrix); 
	JOINTLIBRARY_API void CustomKinematicControllerGetTargetMatrix (const NewtonUserJoint *pick, dFloat* matrix);

	// Generic 6 degree of Freedom Joint
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomJoint6DOF (const dFloat* pinsAndPivotChildFrame, const dFloat* pinsAndPivotParentFrame, const NewtonBody* child, const NewtonBody* parent);
	JOINTLIBRARY_API void CustomJoint6DOF_SetLinearLimits (NewtonUserJoint* customJoint6DOF, const dFloat* minLinearLimits, const dFloat* maxLinearLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_SetAngularLimits (NewtonUserJoint* customJoint6DOF, const dFloat* minAngularLimits, const dFloat* maxAngularLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_GetLinearLimits (NewtonUserJoint* customJoint6DOF, dFloat* minLinearLimits, dFloat* maxLinearLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_GetAngularLimits (NewtonUserJoint* customJoint6DOF, dFloat* minAngularLimits, dFloat* maxAngularLimits);
	JOINTLIBRARY_API void CustomJoint6DOF_SetReverseUniversal (NewtonUserJoint* customJoint6DOF, int order);


	// Interface for a custom BallAndSocket joint with Limits
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomBallAndSocket (const dFloat* pinsAndPivotChildFrame, const NewtonBody* child, const NewtonBody* parent);
	JOINTLIBRARY_API void BallAndSocketSetConeAngle (NewtonUserJoint* ballJoint, dFloat angle);
	JOINTLIBRARY_API void BallAndSocketSetTwistAngle (NewtonUserJoint* ballJoint, dFloat minAngle, dFloat maxAngle);

	// Interface for a custom Hinge joint with Limits
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomHinge (const dFloat* pinsAndPivotChildFrame, const NewtonBody* child, const NewtonBody* parent);
	JOINTLIBRARY_API void HingeEnableLimits(NewtonUserJoint* hingeJoint, int state);
	JOINTLIBRARY_API void HingeSetLimits (NewtonUserJoint* hingeJoint, dFloat minAngle, dFloat maxAngle);
	JOINTLIBRARY_API dFloat HingeGetJointAngle (const NewtonUserJoint* hingeJoint);
	JOINTLIBRARY_API void HingeGetPinAxis (const NewtonUserJoint* hingeJoint, dFloat* pin);
	JOINTLIBRARY_API dFloat HingeCalculateJointOmega (const NewtonUserJoint* hingeJoint);

	// Interface for a custom Slider joint with Limits
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomSlider (const dFloat* pinsAndPivotChildFrame, const NewtonBody* child, const NewtonBody* parent);
	JOINTLIBRARY_API void SliderEnableLimits(NewtonUserJoint* sliderJoint, int state);
	JOINTLIBRARY_API void SliderSetLimits (NewtonUserJoint* sliderJoint, dFloat mindist, dFloat maxdist);



	// player controller functions
//	typedef int (*PlayerCanPuchThisBodyCalback) (NewtonUserJoint *me, const NewtonBody* hitBody);
	JOINTLIBRARY_API NewtonUserJoint *CreateCustomPlayerController (const dFloat* pins, const NewtonBody* player, dFloat maxStairStepFactor, dFloat cushion);
	JOINTLIBRARY_API void CustomPlayerControllerSetVelocity (const NewtonUserJoint* playerController, dFloat forwardSpeed, dFloat sideSpeed, dFloat heading);
	JOINTLIBRARY_API void CustomPlayerControllerGetVisualMaTrix (const NewtonUserJoint* playerController, dFloat* matrix);
	JOINTLIBRARY_API dFloat CustomPlayerControllerGetMaxSlope (const NewtonUserJoint* playerController);
	JOINTLIBRARY_API void CustomPlayerControllerSetMaxSlope (const NewtonUserJoint* playerController, dFloat maxSlopeAngleIndRadian);
	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetSensorShape (const NewtonUserJoint* playerController);
	

//	JOINTLIBRARY_API void CustomPlayerControllerSetPushActorCallback (NewtonUserJoint* playerController, PlayerCanPuchThisBodyCalback callback);
//	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetVerticalSensorShape (NewtonUserJoint* playerController);
//	JOINTLIBRARY_API const NewtonCollision* CustomPlayerControllerGetDynamicsSensorShape (NewtonUserJoint* playerController);

	// Multi rigid BodyCar controller functions
//	JOINTLIBRARY_API NewtonUserJoint *CreateCustomMultiBodyVehicle (const dFloat* frontDir, const dFloat* upDir, const NewtonBody* carBody);
//	JOINTLIBRARY_API int CustomMultiBodyVehicleAddTire (NewtonUserJoint *car, const void* userData, const dFloat* localPosition, 
//													    dFloat mass, dFloat radius, dFloat width,
//													    dFloat suspensionLength, dFloat springConst, dFloat springDamper);
//	JOINTLIBRARY_API int CustomMultiBodyVehicleAddSlipDifferencial (NewtonUserJoint *car, int leftTireIndex, int rightToreIndex, dFloat maxFriction);
//	JOINTLIBRARY_API int CustomMultiBodyVehicleGetTiresCount(NewtonUserJoint *car);
//	JOINTLIBRARY_API const NewtonBody* CustomMultiBodyVehicleGetTireBody(NewtonUserJoint *car, int tireIndex);
//	JOINTLIBRARY_API dFloat CustomMultiBodyVehicleGetSpeed(NewtonUserJoint *car);
//	JOINTLIBRARY_API dFloat CustomMultiBodyVehicleGetTireSteerAngle (NewtonUserJoint *car, int index);
//	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyTorque (NewtonUserJoint *car, int tireIndex, dFloat torque);
//	JOINTLIBRARY_API void CustomMultiBodyVehicleApplySteering (NewtonUserJoint *car, int tireIndex, dFloat angle);
//	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyBrake (NewtonUserJoint *car, int tireIndex, dFloat brakeTorque);
//	JOINTLIBRARY_API void CustomMultiBodyVehicleApplyTireRollingDrag (NewtonUserJoint *car, int index, dFloat angularDampingCoef);


	// BEGIN k00m (Dave Gravel simple ray cast world vehicle)
	typedef void (*DGRaycastVehicleTireTransformCallback) (NewtonUserJoint *car);
    JOINTLIBRARY_API NewtonUserJoint *DGRaycastVehicleCreate (int maxTireCount, const dFloat* cordenateSytemInLocalSpace, NewtonBody* carBody);
	JOINTLIBRARY_API void DGRaycastVehicleAddTire (NewtonUserJoint *car, void *userData, const dFloat* localPosition, dFloat mass, dFloat radius, dFloat width, dFloat friction, dFloat suspensionLength, dFloat springConst, dFloat springDamper, int castMode);
	JOINTLIBRARY_API void DGRaycastVehicleSetTireTransformCallback (NewtonUserJoint *car, DGRaycastVehicleTireTransformCallback callback);
	JOINTLIBRARY_API int DGRaycastVehicleGetTiresCount(NewtonUserJoint *car);
	JOINTLIBRARY_API void* DGRaycastVehicleGetTiresUserData(NewtonUserJoint *car, int tireIndex);
    JOINTLIBRARY_API void DGRaycastVehicleGetTireMatrix(NewtonUserJoint *car, int tireIndex, dFloat* tireMatrix);

	JOINTLIBRARY_API void DGRaycastVehicleInitNormalizeTireLateralForce(NewtonUserJoint *car, int pointsCount, dFloat* const piceSizeStepAxis, dFloat* const normalizedForceValue);
	JOINTLIBRARY_API void DGRaycastVehicleInitNormalizeTireLongitudinalForce(NewtonUserJoint *car, int pointsCount, dFloat* const piceSizeStepAxis, dFloat* const normalizedForceValue);


//	JOINTLIBRARY_API void DGRayCarGetChassisMatrixLocal(NewtonUserJoint *car, dFloat* chassisMatrix);
//	JOINTLIBRARY_API void DGRayCarTireMatrix(NewtonUserJoint *car, int tire, dFloat* tireMatrix);
//	JOINTLIBRARY_API void DGRayCarSuspensionMatrix(NewtonUserJoint *car, int tire, dFloat param, dFloat* SuspensionMatrix);	
//	JOINTLIBRARY_API const NewtonCollision* DGRayCarTireShape(NewtonUserJoint *car, int tireIndex);    
//	JOINTLIBRARY_API dFloat DGRaycastVehicleGetSpeed(NewtonUserJoint *car);
//	JOINTLIBRARY_API void DGRaycastVehicleSetCustomTireBrake (NewtonUserJoint *car, int index, dFloat torque);
//	JOINTLIBRARY_API void DGRaycastVehicleSetCustomTireTorque (NewtonUserJoint *car, int index, dFloat torque);
//	JOINTLIBRARY_API void DGRaycastVehicleSetCustomTireSteerAngleForce (NewtonUserJoint *car, int index, dFloat angle, dFloat turnforce);
//	JOINTLIBRARY_API dFloat DGRaycastVehicleGenerateTiresBrake (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API dFloat DGRaycastVehicleGenerateTiresTorque (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API dFloat DGRaycastVehicleGenerateTiresSteerForce (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API dFloat DGRaycastVehicleGenerateTiresSteerAngle (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarTireMovePointForceFront (NewtonUserJoint *car, int index, dFloat distance);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarTireMovePointForceRight (NewtonUserJoint *car, int index, dFloat distance);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarTireMovePointForceUp (NewtonUserJoint *car, int index, dFloat distance);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarFixDeceleration (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarChassisRotationLimit (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxSteerAngle (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxSteerRate (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxSteerForceRate (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxSteerForce (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxSteerSpeedRestriction (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxBrakeForce (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxTorque (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarMaxTorqueRate (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarEngineSteerDiv (NewtonUserJoint *car, dFloat value);
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarTireSuspenssionHardLimit (NewtonUserJoint *car, int index, dFloat value);		
//	JOINTLIBRARY_API void DGRaycastVehicleSetVarTireFriction (NewtonUserJoint *car, int index, dFloat value);
//	JOINTLIBRARY_API int DGRaycastVehicleGetVehicleOnAir(NewtonUserJoint *car);
//	JOINTLIBRARY_API int DGRaycastVehicleGetTireOnAir(NewtonUserJoint *car, int index);
//	JOINTLIBRARY_API void DGRaycastVehicleDestroy (NewtonUserJoint *car);
//	END




#ifdef __cplusplus 
}
#endif


#endif
