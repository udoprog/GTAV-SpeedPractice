#include "script.h"
#include "keyboard.h"

#include <optional>
#include <sstream>
#include <fstream>
#include <vector>

class Color {
public:
	int r;
	int g;
	int b;
	int a;
};

class Vec3 {
public:
	Vec3() = default;

	Vec3(float x, float y, float z) : x(x), y(y), z(z) {
	}

	Vec3(Vector3 vec) : x(vec.x), y(vec.y), z(vec.z) {
	}

	float Vdist2(Vec3& other) {
		return SYSTEM::VDIST2(x, y, z, other.x, other.y, other.z);
	}

	Vec3 Add(Vec3& other) {
		return {x + other.x, y + other.y, z + other.z};
	}

	float x;
	float y;
	float z;
};

enum CheckpointType {
	CylinderSingleArrow,
	CylinderDoubleArrow,
	CylinderTripleArrow,
	CylinderCycleArrow,
	CylinderCheckerboard,
	CylinderSingleArrow2,
	CylinderDoubleArrow2,
	CylinderTripleArrow2,
	CylinderCycleArrow2,
	CylinderCheckerboard2,
	RingSingleArrow,
	RingDoubleArrow,
	RingTripleArrow,
	RingCycleArrow,
	RingCheckerboard,
	SingleArrow,
	DoubleArrow,
	TripleArrow,
	CycleArrow,
	Checkerboard,
	CylinderSingleArrow3,
	CylinderDoubleArrow3,
	CylinderTripleArrow3,
	CylinderCycleArrow3,
	CylinderCheckerboard3,
	CylinderSingleArrow4,
	CylinderDoubleArrow4,
	CylinderTripleArrow4,
	CylinderCycleArrow4,
	CylinderCheckerboard4,
	CylinderSingleArrow5,
	CylinderDoubleArrow5,
	CylinderTripleArrow5,
	CylinderCycleArrow5,
	CylinderCheckerboard5,
	RingPlaneUp,
	RingPlaneLeft,
	RingPlaneRight,
	RingPlaneDown,
	Empty,
	Ring,
	Empty2,
	//CylinderCustomShape,
	//CylinderCustomShape2,
	//CylinderCustomShape3,
	Cyclinder = 45,
	Cyclinder2,
	Cyclinder3,
};

enum BlipColor {
	White1,
	Red,
	Green,
	Blue1,
	White2,
    TaxiYellow,
	LightRed1,
	Violet,
	Pink,
	LightOrange,
	LightBrown1,
	LightGreen,
	LightBlue1,
	VeryLightPurple,
	DarkPurple,
	Cyan,
	LightYellow1,
	Orange,
	LightBlue2,
	DarkPink,
	DarkYellow,
	DarkOrange,
	LightGray1,
	LightPink1,
	LemonGreen,
	ForestGreen,
	ElectricBlue,
	BrightPurple,
	DarkTaxiYellow,
	DarkBlue,
	DarkCyan,
	LightBrown2,
	VeryLightBlue,
	LightYellow2,
	LightPink2,
	LightRed2,
	LightYellow3,
	White3,
	Blue2,
	LightGray2,
	DarkGray
};

enum CheckpointKind {
	Waypoint,
	PowerPoint,
	BreakBoost,
	End,
};

class CheckpointConfig {
public:
	CheckpointType Type;
	Color Color;
	float Radius;
	Vec3 Offset;
	BlipColor BlipColor;
};

class CheckpointPosition {
public:
	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		Vec3 PointTo
	) : Kind(Kind), Pos(Pos), PointTo(PointTo), Radius(10) {
	}

	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		Vec3 PointTo,
		float Radius
	) : Kind(Kind), Pos(Pos), PointTo(PointTo), Radius(Radius) {
	}

	CheckpointConfig Config() {
		float radius = this->Radius;
		float offsetZ = 2 * (10 / radius);

		switch (Kind) {
		case Waypoint:
			return { RingSingleArrow, { 255, 255, 255, 64 }, radius, { 0, 0, offsetZ }, White1 };
		case PowerPoint:
			return { RingSingleArrow, { 255, 255, 0, 64 }, radius, { 0, 0, offsetZ }, LightYellow1 };
		case BreakBoost:
			return { RingSingleArrow, { 255, 0, 0, 64 }, radius, { 0, 0, offsetZ }, Red };
		case End:
			return { Cyclinder, { 255, 255, 255, 64 }, 10, { 0, 0, -2 }, LightYellow2 };
		}
	}

	CheckpointKind Kind;
	Vec3 Pos;
	Vec3 PointTo;
	float Radius;
};

class Checkpoint {
public:
	Checkpoint(CheckpointType type, Vec3 pos, Vec3 pointTo, float radius, Color color, Vec3 checkPos, BlipColor blipColor) {
		Pos = pos;
		CheckPos = checkPos;
		Radius2 = radius * radius;
		handle = GRAPHICS::CREATE_CHECKPOINT(type, pos.x, pos.y, pos.z, pointTo.x, pointTo.y, pointTo.z, radius, color.r, color.g, color.b, color.a, 0);
		blip = UI::ADD_BLIP_FOR_RADIUS(pos.x, pos.y, pos.z, radius);
		UI::SET_BLIP_COLOUR(blip, blipColor);
	}

	~Checkpoint() {
		GRAPHICS::DELETE_CHECKPOINT(handle);
		UI::REMOVE_BLIP(&blip);
	}

	Vec3 Pos;
	Vec3 CheckPos;
	float Radius2;

private:
	int handle;
	Blip blip;
};

class Mission {
public:
	Mission(std::string name) : Name(name), Current(0), positions() {
	}

	void Push(CheckpointPosition position) {
		positions.push_back(position);
	}

	// Decrement the current checkpoint.
	Checkpoint* Decrement() {
		Current -= 2;

		if (Current < 0) {
			Current = 0;
		}

		return Advance();
	}

	// Advance the current checkpoint.
	Checkpoint* Advance() {
		if (this->Current >= positions.size()) {
			return nullptr;
		}

		auto current = positions[this->Current];
		this->Current += 1;

		auto drawPos = current.Pos;
		auto pointTo = current.PointTo;
		auto config = current.Config();

		// TODO: implement arithmetics
		drawPos = drawPos.Add(config.Offset);
		pointTo = pointTo.Add(config.Offset);

		auto checkPos = current.Pos;

		return new Checkpoint(config.Type, drawPos, pointTo, config.Radius, config.Color, checkPos, config.BlipColor);
	}

	// Reset the mission.
	Checkpoint* Reset() {
		Current = 0;

		std::ostringstream s;
		s << "Training: ~b~" << Name << "~w~";
		Notify(s.str().c_str());

		return Advance();
	}

	std::string Name;
	int Current;

private:
	std::vector<CheckpointPosition> positions;
};

// Show a subtitle.
void ShowSubtitle(const char* s) {
	UI::_SET_TEXT_ENTRY_2("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((char *)s);
	UI::_DRAW_SUBTITLE_TIMED(2000, true);
}

// Draw a simple form of notification next to the map.
void Notify(const char* s) {
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING3((char*)s);
	UI::_DRAW_NOTIFICATION(true, true);
}

void DebugPosition(Ped ped, Mission* current) {
	auto coords = ENTITY::GET_ENTITY_COORDS(ped, true);
	auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(ped);

	std::ostringstream stream;

	stream << "Training: ~y~" << current->Name << "~w~";
	stream << " current: " << current->Current << "\n";

	stream
		<< "xyz (" << coords.x << ", " << coords.y << ", " << coords.z << "), "
		<< "-> (" << forward.x << ", " << forward.y << ", " << forward.z << ")";

	std::string s = stream.str();
	ShowSubtitle(s.c_str());
}

void DebugMissionEnd() {
	std::string name = "?";

	if (SCRIPT::HAS_SCRIPT_LOADED("franklin0")) {
		name = "Franklin & Lamar";
	} else if (SCRIPT::HAS_SCRIPT_LOADED("armenian2")) {
		name = "Repossession";
	}

	std::ostringstream stream;
	stream << "Mission: " << AUDIO::IS_MISSION_COMPLETE_PLAYING() << "flag: " << GAMEPLAY::GET_MISSION_FLAG() << " (" << name << ")";
	std::string s = stream.str();
	ShowSubtitle(s.c_str());
}

void ScriptMain() {
	bool debug = false;
	float ticker = 0.0;

	int missionIndex = 0;

	Mission* missions[] = {
		FranklinAndLamar(),
		nullptr,
	};

	Mission* current = missions[missionIndex];

	Checkpoint* point = nullptr;
	
	if (current != nullptr) {
		point = current->Advance();
	}

	while (true) {
		if (IsKeyJustUp(VK_F4, true)) {
			debug = !debug;
		}

		if (IsKeyJustUp(VK_LEFT, true)) {
			missionIndex -= 1;

			if (missionIndex < 0) {
				missionIndex = 0;
			}

			current = missions[missionIndex];
			delete point;
			point = current->Reset();
		}

		if (IsKeyJustUp(VK_RIGHT, true)) {
			if (missions[missionIndex + 1] != nullptr) {
				missionIndex += 1;
				current = missions[missionIndex];
				delete point;
				point = current->Reset();
			}
		}

		if (IsKeyJustUp(VK_DOWN, true)) {
			if (current != nullptr) {
				delete point;
				point = current->Decrement();
			}
		}

		if (IsKeyJustUp(VK_UP, true)) {
			if (current != nullptr) {
				delete point;
				point = current->Advance();
			}
		}

		ticker += GAMEPLAY::GET_FRAME_TIME();

		Player player = PLAYER::PLAYER_ID();
		Ped ped = PLAYER::GET_PLAYER_PED(player);

		Vec3 coords = ENTITY::GET_ENTITY_COORDS(ped, true);

		if (IsKeyJustUp(VK_DELETE, true)) {
			if (current != nullptr) {
				delete point;
				point = current->Advance();

				std::ofstream os;
				os.open("coords.txt", std::ofstream::out | std::ofstream::app);

				os << "{" << coords.x << ", " << coords.y << ", " << coords.z << "}" << std::endl;
			}
		}

		if (point != nullptr) {
			if (point->CheckPos.Vdist2(coords) < point->Radius2) {
				delete point;

				if (current != nullptr) {
					point = current->Advance();
				}
			}
		}

		if (debug) {
			if (ticker > 0.05) {
				if (current != nullptr) {
					DebugPosition(ped, current);
				}

				ticker = 0;
			}
		}

		WAIT(0);
	}
}

Mission* FranklinAndLamar() {
	Mission *points = new Mission("Franklin & Lamar");

	points->Push({ Waypoint, {-1886.48, -561.961, 11.2672}, {-2015.77, -455.14, 11.0581} });

	points->Push({ Waypoint, {-2015.77, -455.14, 11.0581}, {-1994, -419, 12} });

	points->Push({ Waypoint, {-1994, -419, 12}, {-1955, -424, 17} });
	points->Push({ Waypoint, {-1857, -487, 26}, {-1813, -517, 30} });

	// power point to prevent from going off the top of the hill.
	points->Push({ PowerPoint, {-1755.92, -550.803, 36.2118}, {-1733.77, -557.012, 36.7685} });
	points->Push({ Waypoint, {-1712, -563, 36}, {-1643, -597, 33} });
	points->Push({ BreakBoost, {-1637, -596, 33}, {-1635, -597, 33} });
	points->Push({ PowerPoint, {-1535, -642, 28}, {-1492, -631, 30} });
	points->Push({ BreakBoost, {-1479, -618, 30}, {-1474, -614.5, 30.5} });

	// intersection 1
	points->Push({ Waypoint, {-1316.15, -513.217, 32.6707}, {-1363, -550, 30} });
	// intersection 2
	points->Push({ Waypoint, {-1241, -448, 33}, {-1181, -420.5, 34} });
	// turn before studio
	points->Push({ Waypoint, {-1099, -414, 36}, {-1075, -434, 36} });
	// entrance to studio
	points->Push({ Waypoint, {-1055.55, -469.124, 35.8914}, {-1061.56, -483.848, 36.102} });
	// inside studio
	points->Push({ Waypoint, {-1113.38, -512.257, 34.2677}, {-1133.38, -522.442, 32.8011} });
	// exit to studio
	points->Push({ Waypoint, {-1207.22, -580.16, 26.6866}, {-1198.44, -633.651, 23.7369} });
	// after studio
	points->Push({ Waypoint, {-1173.97, -666.67, 22.5446}, {-1143.39, -700.038, 20.895}, 30 });
	// turn towards truck
	points->Push({ Waypoint, {-1108.82, -761.182, 18.7327}, {-1132.03, -813.194, 15.3551} });
	// turn down towards building site
	points->Push({ Waypoint, {-1157.63, -849.399, 13.7151}, {-1151.35, -876.797, 10.5292} });
	// first jump
	points->Push({ Waypoint, {-1090.76, -979.072, 3.9196}, {-1037.43, -1071.6, 3.66611} });
	points->Push({ Waypoint, {-1037.43, -1071.6, 3.66611}, {-979.515, -1162.63, 3.63968} });
	points->Push({ PowerPoint, {-979.515, -1162.63, 3.63968}, {-946.05, -1194.59, 4.64777} });
	// turn up towards vespucci road.
	points->Push({ PowerPoint, {-946.381, -1197.46, 4.65006}, {-907.4, -1188.37, 4.62585} });
	// intersection
	points->Push({ Waypoint, {-797.602, -1122.58, 10.2477}, {-762.629, -1104.09, 10.3714} });
	// hill up
	points->Push({ Waypoint, {-679.172, -1049.98, 15.7689}, {-659.784, -1026.22, 17.745} });
	// turn towards highway
	points->Push({ PowerPoint, {-619.136, -877.421, 24.6516}, {-597.697, -856.391, 25.2748} });

	points->Push({ Waypoint, {-518.104, -837.862, 30.0275}, {-482.834, -843.755, 30.0885}, 30 });

	points->Push({ PowerPoint, {-295.956, -835.602, 31.3429}, {-266.187, -797.451, 31.7652} });

	points->Push({ Waypoint, {-222.812, -695.03, 33.1329}, {-183.073, -679.134, 33.8081} });
	points->Push({ Waypoint, {-86.6128, -677.583, 34.8648}, {-75.0701, -681.3, 33.5304} });

	points->Push({ PowerPoint, {63.1746, -625.133, 31.2793}, {-54.6064, -756, 31.257} });
	points->Push({ Waypoint, {54.6064, -756.0, 31.257}, {135.0, -808.832, 30.8428} });

	points->Push({ Waypoint, {155.112, -831.111, 30.6528}, {108.104, -979.07, 28.9833} });

	points->Push({ Waypoint, {108.104, -979.07, 28.9833}, {71.3636, -1053.01, 28.9159} });
	// pointing into the alley.
	points->Push({ Waypoint, {71.3636, -1053.01, 28.9159}, {-32.0474, -1020.51, 28.439} });
	// down the alley with the cat.
	points->Push({ Waypoint, {-32.0474, -1020.51, 28.439}, {-51.1589, -1076.17, 26.518} });

	// Simeon's
	points->Push({ End, {-51.1589, -1076.17, 26.518}, {0, 0, 0} });
	return points;
}