#pragma once

// Makes converting from RGB to a Pixel easy
union Color {
	Pixel pixel;
	struct
	{
		Pixel b : 8;
		Pixel g : 8;
		Pixel r : 8;
		Pixel a : 8;
	} c;
};