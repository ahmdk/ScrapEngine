#include <Engine/LogicCore/GameObject/SGameObject.h>
#include <algorithm>

ScrapEngine::Core::SGameObject::SGameObject(const std::string& object_name,
                                            const ScrapEngine::Transform& input_object_transform,
                                            const bool is_static_object) :
	SObject(object_name), object_transform_(input_object_transform), is_static_(is_static_object)
{
}

ScrapEngine::Core::SGameObject::~SGameObject()
{
	for (SComponent* component : object_components_)
	{
		delete component;
	}
}

void ScrapEngine::Core::SGameObject::game_start()
{
	//This will be defined by the user when is necessary, otherwise it will have no effect
}

void ScrapEngine::Core::SGameObject::game_update(float time)
{
	//This will be defined by the user when is necessary, otherwise it will have no effect
}

void ScrapEngine::Core::SGameObject::set_object_location(const glm::vec3& location, const bool should_update_relative)
{
	object_transform_.location = location;
	if (should_update_relative)
	{
		update_relative_transform();
	}
	//Update the transform of every component
	for (SComponent* component : object_components_)
	{
		component->set_father_transform(object_transform_);
		component->update_component_location();
	}
	//Update the transform of every child
	for (SGameObject* child_object : object_child_)
	{
		child_object->update_object_location();
	}
}

void ScrapEngine::Core::SGameObject::set_object_rotation(const glm::vec3& rotation, const bool should_update_relative)
{
	object_transform_.rotation = rotation;
	if (should_update_relative)
	{
		update_relative_transform();
	}
	//Update the transform of every component
	for (SComponent* component : object_components_)
	{
		component->set_father_transform(object_transform_);
		component->update_component_rotation();
	}
	//Update the transform of every child
	for (SGameObject* child_object : object_child_)
	{
		child_object->update_object_rotation();
	}
}

void ScrapEngine::Core::SGameObject::set_object_scale(const glm::vec3& scale, const bool should_update_relative)
{
	object_transform_.scale = scale;
	if (should_update_relative)
	{
		update_relative_transform();
	}
	//Update the transform of every component
	for (SComponent* component : object_components_)
	{
		component->set_father_transform(object_transform_);
		component->update_component_scale();
	}
	//Update the transform of every child
	for (SGameObject* child_object : object_child_)
	{
		child_object->update_object_scale();
	}
}

glm::vec3 ScrapEngine::Core::SGameObject::get_object_location() const
{
	return object_transform_.location;
}

glm::vec3 ScrapEngine::Core::SGameObject::get_object_rotation() const
{
	return object_transform_.rotation;
}

glm::vec3 ScrapEngine::Core::SGameObject::get_object_scale() const
{
	return object_transform_.scale;
}

void ScrapEngine::Core::SGameObject::update_relative_transform()
{
	if (father_object_)
	{
		object_relative_transform_.location = object_transform_.location - father_object_->object_transform_.location;
		object_relative_transform_.rotation = object_transform_.rotation - father_object_->object_transform_.rotation;
		object_relative_transform_.scale = object_transform_.scale - father_object_->object_transform_.scale;
	}
}

void ScrapEngine::Core::SGameObject::update_object_location()
{
	const glm::mat4 local_m = generate_unscaled_transform_matrix(object_relative_transform_);
	const glm::mat4 father_m = generate_unscaled_transform_matrix(father_object_->object_transform_);
	glm::mat4 full_m = father_m * local_m;

	glm::vec3 pos = glm::vec3(full_m[3][0], full_m[3][1], full_m[3][2]);

	set_object_location(pos, false);
}

void ScrapEngine::Core::SGameObject::update_object_rotation()
{
	object_transform_.rotation = father_object_->object_transform_.rotation + object_relative_transform_.rotation;

	update_object_location();
	set_object_rotation(object_transform_.rotation, false);
}

void ScrapEngine::Core::SGameObject::update_object_scale()
{
	object_transform_.scale = father_object_->object_transform_.scale + object_relative_transform_.scale;
}

void ScrapEngine::Core::SGameObject::add_component(SComponent* component)
{
	object_components_.push_back(component);
	//Set component default values same as object
	component->set_component_location(object_transform_.location);
	component->set_component_rotation(object_transform_.rotation);
	component->set_component_scale(object_transform_.scale);
}

void ScrapEngine::Core::SGameObject::remove_component(SComponent* component)
{
	object_components_.erase(std::remove(
		                         object_components_.begin(),
		                         object_components_.end(),
		                         component),
	                         object_components_.end());
}

const std::vector<ScrapEngine::Core::SComponent*>* ScrapEngine::Core::SGameObject::get_components() const
{
	return &object_components_;
}

void ScrapEngine::Core::SGameObject::add_child(SGameObject* game_object)
{
	object_child_.push_back(game_object);
	game_object->father_object_ = this;
	game_object->update_relative_transform();
}

void ScrapEngine::Core::SGameObject::remove_child(SGameObject* game_object)
{
	object_child_.erase(std::remove(
		                    object_child_.begin(),
		                    object_child_.end(),
		                    game_object),
	                    object_child_.end());
	game_object->father_object_ = nullptr;
}

const std::vector<ScrapEngine::Core::SGameObject*>* ScrapEngine::Core::SGameObject::get_child() const
{
	return &object_child_;
}
