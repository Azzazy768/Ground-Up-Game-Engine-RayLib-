#pragma once
#include "../Core/Component.h"
#include "TransformComponent.h"
#include "raylib.h"
#include "rlgl.h" //Needed to add rotation to my object visually

class TankRendererComponent : public Component {
	TransformComponent* transformOnTank;

public:

	TankRendererComponent(TransformComponent* transform) {
		transformOnTank = transform;
	}
		
	void Render() {

		if (transformOnTank != nullptr) {

			//The following implementation was found online. Here's the gist of what i understood about it. rlPushMatrix saves the current world state in a stack.
			//Then you move the center of the world to be tanks position using Translatef
			//you then rotate the object using rotatef, which takes the y rotation from the transform object.
			//you then draw the tank at {0,0,0} because conceptually, the tank needs to be drawn at the origin of the world coords, the ones we changed to be the tanks position
			//after you draw, you pop the stack, resetting the world state to what it is (making the center of the world back to what it was)
			rlPushMatrix();
			rlTranslatef(transformOnTank->position.x, transformOnTank->position.y, transformOnTank->position.z);
			rlRotatef(transformOnTank->rotation.y * RAD2DEG, 0, 1, 0); //Had to switch the rotation from radians to degrees (RAD2DEG)

			
			DrawCubeWires({0,0,0}, 2.0f, 1.0f, 3.0f, GREEN); //Tank Drawing
			DrawCubeWires({0,0.6,0}, 1.0f, 0.5f, 1.5f, RED); //Turret drawing (making it 0.6 units higher than the tank)
			rlPopMatrix();
		}
	}
};