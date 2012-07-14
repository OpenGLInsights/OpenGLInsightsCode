#version 420 core

layout(location = 0) flat out int instanceID;

void main(void) {

	/* pass through instance ID */
	instanceID = gl_InstanceID;

}
