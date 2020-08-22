R""(
void	ApplyTechnique()
{
	if (Frag.Material.Alpha <= 0.05f)
		discard;
}

)""