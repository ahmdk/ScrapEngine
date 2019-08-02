#include "Ball.h"
#include <Engine/Input/KeyboardKeys.h>

Ball::Ball(ScrapEngine::Core::ComponentsManager* input_component_manager,
           ScrapEngine::Input::InputManager* created_input_managerf)
	: SGameObject("Ball game object"), input_manager_ref_(created_input_managerf)
{
	//Add mesh to that GameObject
	set_object_scale(ScrapEngine::Core::SVector3(0.5f, 0.5f, 0.5f));
	set_spawnpoint(ScrapEngine::Core::SVector3(0, -10, 0));

	mesh_ = input_component_manager->create_new_mesh_component(
		"../assets/shader/compiled_shaders/shader_base.vert.spv",
		"../assets/shader/compiled_shaders/shader_base.frag.spv",
		"../assets/models/sphere.obj",
		{"../assets/textures/SimpleBallTexture.png"}
	);
	add_component(mesh_);

	collider_ = input_component_manager->create_sphere_rigidbody_component(
		10.f,
		ScrapEngine::Core::SVector3(0, -10, 0), 8.f);
	add_component(collider_);

	collider_->attach_to_mesh(mesh_);

	collider_->set_friction_coefficient(1);
}

void Ball::game_update(const float delta_time)
{
	if (can_move_)
	{
		if(get_rigidbody_location().get_y() > 10)
		{
			return;
		}
		if (input_manager_ref_->get_keyboard_key_pressed(KEYBOARD_KEY_ARROW_UP))
		{
			collider_->apply_force_to_center(ScrapEngine::Core::SVector3(0, 0, -700 * (delta_time * 100)));
		}
		if (input_manager_ref_->get_keyboard_key_pressed(KEYBOARD_KEY_ARROW_DOWN))
		{
			collider_->apply_force_to_center(ScrapEngine::Core::SVector3(0, 0, 700 * (delta_time * 100)));
		}
		if (input_manager_ref_->get_keyboard_key_pressed(KEYBOARD_KEY_ARROW_LEFT))
		{
			collider_->apply_force_to_center(ScrapEngine::Core::SVector3(-700 * (delta_time * 100), 0, 0));
		}
		if (input_manager_ref_->get_keyboard_key_pressed(KEYBOARD_KEY_ARROW_RIGHT))
		{
			collider_->apply_force_to_center(ScrapEngine::Core::SVector3(700 * (delta_time * 100), 0, 0));
		}
	}
}

void Ball::respawn()
{
	collider_->cancel_rigidbody_forces();
	collider_->modify_rigidbody_location(respawn_point_);
}

void Ball::set_can_move(const bool can_move)
{
	can_move_ = can_move;
	if (can_move)
	{
		input_manager_ref_->set_cursor_input_mode(ScrapEngine::Input::cursor_mode::cursor_grabbed_mode);
	}
	else
	{
		input_manager_ref_->set_cursor_input_mode(ScrapEngine::Input::cursor_mode::cursor_normal_mode);
	}
}

bool Ball::get_can_move() const
{
	return can_move_;
}

void Ball::set_spawnpoint(const ScrapEngine::Core::SVector3& pos)
{
	respawn_point_ = pos;
}

ScrapEngine::Core::SVector3 Ball::get_rigidbody_location() const
{
	return mesh_->get_component_location();
}