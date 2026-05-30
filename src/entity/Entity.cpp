#include "Entity.hpp"

Entity::Entity(Model model, glm::dvec3 pos, glm::quat rot): model(std::move(model)) {
    this->position = pos;
    this->orientation = rot;
}

void Entity::apply(float dt) {
    RigidBody::apply(dt);

    this->model_matrix = glm::dmat4(1.0f);
    this->model_matrix = glm::translate(this->model_matrix, position);
    this->model_matrix *= glm::dmat4(glm::mat4_cast(orientation));
}

void Entity::drawOpaque(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) {
    this->model.drawOpaque(shader, this->model_matrix, view, projection);
}

void Entity::drawTransparent(GLuint shader, const glm::dmat4& view, const glm::mat4& projection) {
    this->model.drawTransparent(shader, this->model_matrix, view, projection);
}