R""(
uniform uint DrawID;
layout(location = 5) out uint	out_InstanceID;

void SetInstanceID() {
	out_InstanceID = DrawID;
}

)""