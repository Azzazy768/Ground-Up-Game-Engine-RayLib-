#pragma once
#include "../Core/Component.h"
#include "raylib.h"
#include "raymath.h"
#include "json.hpp"


class TransformComponent : public Component {
public:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Vector3 orginalPostion;

    TransformComponent() { //I'm setting the default position values for the creation of the transform component. Will happen when object is created etc. 
        position = { 0.0f, 0.0f, 0.0f };
        rotation = { 0.0f, 0.0f, 0.0f };
        scale = { 1.0f, 1.0f, 1.0f };
        orginalPostion = { 0.0f,0.0f,0.0f };
    }

    void Translate(Vector3 translate) {
        position.x += translate.x;
        position.y += translate.y;
        position.z += translate.z;
    }

    void Rotate(Vector3 inputRotation) {
        rotation.x += inputRotation.x;
        rotation.y += inputRotation.y;
        rotation.z += inputRotation.z;
    }

    Matrix getWorldPos() { //I added this so that i can combine all the transformations and calculate the final position of the vectors. I take all of the vec3's i made, turn them to matricies, multiply them to get the final position. Using Translate * Rotation * Scale (as i learnt in graphics)
        Matrix matScale = MatrixScale(
            scale.x,
            scale.y,
            scale.z
        );
        Matrix matRotate = MatrixRotateY(rotation.y);
        
        Matrix matTranslate = MatrixTranslate(
            position.x,
            position.y,
            position.z
        );

        Matrix matWorld = MatrixMultiply(MatrixMultiply(matScale, matRotate), matTranslate);
        return matWorld;
    }


    void Update(float deltaTime){

    }

    void Render() {}

    static Component* CreateComponent(void* pData, GameObject* pOwner) {
        nlohmann::json* j = static_cast<nlohmann::json*>(pData);
        TransformComponent* transform = new TransformComponent();

        if (j->contains("Position")) {
            transform->position.x = (*j)["Position"][0]; //Goes into the json file. Finds Position, and takes the first part of the array (the x basiaclly)
            transform->position.y = (*j)["Position"][1];
            transform->position.z = (*j)["Position"][2];
        }
        if (j->contains("Rotation")) {
            transform->rotation.x = (*j)["Rotation"][0]; 
            transform->rotation.y = (*j)["Rotation"][1];
            transform->rotation.z = (*j)["Rotation"][2];
        }
        if (j->contains("Scale")) {
            transform->scale.x = (*j)["Scale"][0]; 
            transform->scale.y = (*j)["Scale"][1];
            transform->scale.z = (*j)["Scale"][2];
        }

        return transform;
    }

};
