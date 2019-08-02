#include <Engine/Physics/RigidBody/RigidBody.h>
#include <Engine/Debug/DebugLog.h>
#include <glm/gtc/quaternion.inl>

ScrapEngine::Core::STransform ScrapEngine::Physics::RigidBody::convert_transform(const rp3d::Transform& other)
{
	Core::STransform return_tras;

	//Convert position
	const rp3d::Vector3& other_pos = other.getPosition();
	return_tras.set_position(Core::SVector3(other_pos.x, other_pos.y, other_pos.z));

	//Convert rotation
	const rp3d::Quaternion& rot = other.getOrientation();
	const glm::quat glm_quat(rot.w, rot.x, rot.y, rot.z);
	return_tras.set_rotation(Core::SQuaternion(glm_quat));

	return return_tras;
}

rp3d::Transform ScrapEngine::Physics::RigidBody::convert_transform(const Core::STransform& other)
{
	const rp3d::Vector3 pos = convert_vector(other.get_position());

	const Core::SVector3& other_rot = other.get_rotation();
	const rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(
		other_rot.get_x(),
		other_rot.get_y(),
		other_rot.get_z()
	);

	return rp3d::Transform(pos, rotation);
}

rp3d::Vector3 ScrapEngine::Physics::RigidBody::convert_vector(const Core::SVector3& vector)
{
	return rp3d::Vector3(vector.get_x(), vector.get_y(), vector.get_z());
}

ScrapEngine::Physics::RigidBody::~RigidBody()
{
	delete shape_;
}

void ScrapEngine::Physics::RigidBody::set_start_transform(const rp3d::Vector3& init_position,
                                                          const rp3d::Quaternion& init_orientation)
{
	transform_ = rp3d::Transform(init_position, init_orientation);
}

void ScrapEngine::Physics::RigidBody::set_collision_shape(CollisionShape* shape)
{
	if (proxy_shape_)
	{
		body_->removeCollisionShape(proxy_shape_);
	}
	delete shape_;

	shape_ = shape;
	proxy_shape_ = body_->addCollisionShape(shape_->get_collision_shape(),
	                                        rp3d::Transform::identity(), get_mass());
}

void ScrapEngine::Physics::RigidBody::set_mass(const float mass) const
{
	body_->setMass(mass);
}

float ScrapEngine::Physics::RigidBody::get_mass() const
{
	if (body_)
	{
		return body_->getMass();
	}
	return 0;
}

void ScrapEngine::Physics::RigidBody::set_type(const RigidBody_Types type) const
{
	switch (type)
	{
	case static_rigidbody:
		body_->setType(rp3d::BodyType::STATIC);
		break;
	case kinematic_rigidbody:
		body_->setType(rp3d::BodyType::KINEMATIC);
		break;
	default:
	case dynamic_rigidbody:
		body_->setType(rp3d::BodyType::DYNAMIC);
		break;
	}
}

ScrapEngine::Physics::RigidBody_Types ScrapEngine::Physics::RigidBody::get_type() const
{
	switch (body_->getType())
	{
	case rp3d::BodyType::STATIC:
		return static_rigidbody;
	case rp3d::BodyType::KINEMATIC:
		return kinematic_rigidbody;
	default:
	case rp3d::BodyType::DYNAMIC:
		return dynamic_rigidbody;
	}
}

void ScrapEngine::Physics::RigidBody::build_rigidbody(rp3d::DynamicsWorld* dynamic_world)
{
	body_ = dynamic_world->createRigidBody(transform_);
	body_->setMass(1.f);
}

void ScrapEngine::Physics::RigidBody::remove_from_world(rp3d::DynamicsWorld* dynamic_world) const
{
	dynamic_world->destroyRigidBody(body_);
}

rp3d::RigidBody* ScrapEngine::Physics::RigidBody::get_rigidbody() const
{
	return body_;
}

ScrapEngine::Core::STransform ScrapEngine::Physics::RigidBody::get_updated_transform(const float factor)
{
	const rp3d::Transform curr_transform = body_->getTransform();
	const rp3d::Transform interpolated_transform = rp3d::Transform::interpolateTransforms(
		prev_transform_,
		curr_transform,
		factor);
	Core::STransform return_tras = convert_transform(interpolated_transform);

	prev_transform_ = curr_transform;
	return return_tras;
}

void ScrapEngine::Physics::RigidBody::set_new_transform(const Core::STransform& transform) const
{
	body_->setTransform(convert_transform(transform));
}

void ScrapEngine::Physics::RigidBody::set_new_location(const Core::SVector3& location) const
{
	rp3d::Transform current_transform = body_->getTransform();
	current_transform.setPosition(convert_vector(location));
	body_->setTransform(current_transform);
}

float ScrapEngine::Physics::RigidBody::get_bounciness() const
{
	// Get the current material of the body
	rp3d::Material& material = body_->getMaterial();

	return material.getBounciness();
}

void ScrapEngine::Physics::RigidBody::set_bounciness(const float bounce_factor) const
{
	// Get the current material of the body
	rp3d::Material& material = body_->getMaterial();

	material.setBounciness(rp3d::decimal(bounce_factor));
}

float ScrapEngine::Physics::RigidBody::get_friction_coefficient() const
{
	// Get the current material of the body
	rp3d::Material& material = body_->getMaterial();

	return material.getFrictionCoefficient();
}

void ScrapEngine::Physics::RigidBody::set_friction_coefficient(const float coefficient) const
{
	// Get the current material of the body
	rp3d::Material& material = body_->getMaterial();

	material.setFrictionCoefficient(rp3d::decimal(coefficient));
}

bool ScrapEngine::Physics::RigidBody::get_allowed_to_sleep() const
{
	return body_->isAllowedToSleep();
}

void ScrapEngine::Physics::RigidBody::set_allowed_to_sleep(const bool allowed) const
{
	body_->setIsAllowedToSleep(allowed);
}

void ScrapEngine::Physics::RigidBody::apply_force_to_center(const Core::SVector3& force) const
{
	body_->applyForceToCenterOfMass(convert_vector(force));
}

void ScrapEngine::Physics::RigidBody::apply_torque(const Core::SVector3& force) const
{
	body_->applyTorque(convert_vector(force));
}

void ScrapEngine::Physics::RigidBody::cancel_rigidbody_forces() const
{
	body_->setAngularVelocity(rp3d::Vector3(0, 0, 0));
	body_->setLinearVelocity(rp3d::Vector3(0, 0, 0));
}