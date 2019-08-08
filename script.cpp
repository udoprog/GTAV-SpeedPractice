#include "script.h"
#include "keyboard.h"

#include <optional>
#include <sstream>
#include <fstream>
#include <vector>

Mission FranklinAndLamar();
Mission Chop();
Mission Complications();

enum Option {
	None,
	DisableOffset,
};

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
	Marker,
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
	Option Option;
};

class CheckpointPosition {
public:
	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos
	) : Kind(Kind), Pos(Pos), PointTo(), Radius(10), option(None) {
	}

	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		float Radius
	) : Kind(Kind), Pos(Pos), PointTo(), Radius(Radius), option(None) {
	}

	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		float Radius,
		Option Option
	) : Kind(Kind), Pos(Pos), PointTo(), Radius(Radius), option(Option) {
	}

	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		Vec3 PointTo
	) : Kind(Kind), Pos(Pos), PointTo(PointTo), Radius(10), option(None) {
	}

	CheckpointPosition(
		CheckpointKind Kind,
		Vec3 Pos,
		Vec3 PointTo,
		float Radius
	) : Kind(Kind), Pos(Pos), PointTo(PointTo), Radius(Radius), option(None) {
	}

	CheckpointConfig Config() {
		float radius = this->Radius;
		float offsetZ = 2 * (10 / radius);

		switch (Kind) {
		case Marker:
			return { Cyclinder, { 255, 255, 255, 128 }, radius, { 0, 0, -2 }, White1, option };
		case Waypoint:
			return { RingSingleArrow, { 255, 255, 255, 64 }, radius, { 0, 0, offsetZ }, White1, option };
		case PowerPoint:
			return { RingSingleArrow, { 255, 255, 0, 64 }, radius, { 0, 0, offsetZ }, LightYellow1, option };
		case BreakBoost:
			return { RingSingleArrow, { 255, 0, 0, 64 }, radius, { 0, 0, offsetZ }, Red, option };
		case End:
			return { Cyclinder, { 255, 255, 255, 64 }, 10, { 0, 0, -2 }, LightYellow2, option };
		}
	}

	CheckpointKind Kind;
	Vec3 Pos;
	// where to point the current checkpoint. If absent, will point to the next point in the list.
	std::optional<Vec3> PointTo;
	float Radius;

private:
	Option option;
};

class Checkpoint {
public:
	Checkpoint(CheckpointType type, Vec3 pos, Vec3 pointTo, float radius, Color color, Vec3 checkPos, BlipColor blipColor) {
		Pos = pos;
		CheckPos = checkPos;
		Radius2 = radius * radius;
		handle = GRAPHICS::CREATE_CHECKPOINT(type, pos.x, pos.y, pos.z, pointTo.x, pointTo.y, pointTo.z, radius, color.r, color.g, color.b, color.a, 0);
		blip = UI::ADD_BLIP_FOR_RADIUS(pos.x, pos.y, pos.z, radius / 2);
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
	Mission() = delete;
	Mission(Mission&) = delete;
	Mission(Mission&&) = default;

	Mission(std::string name)
		: Name(name),
		  Current(0),
		  positions()
	{
	}

	void Push(const CheckpointPosition& position) {
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
		if (Current >= positions.size()) {
			return nullptr;
		}

		auto current = positions[Current];
		Current += 1;

		auto drawPos = current.Pos;
		auto maybePointTo = current.PointTo;
		auto config = current.Config();

		if (config.Option != DisableOffset) {
			drawPos = drawPos.Add(config.Offset);
		}

		auto pointTo = drawPos;

		if (maybePointTo) {
			pointTo = (*maybePointTo);

			if (config.Option != DisableOffset) {
				pointTo = pointTo.Add(config.Offset);
			}
		}
		else if (Current < positions.size()) {
			auto next = positions[Current];
			auto nextConfig = next.Config();

			pointTo = next.Pos;

			if (nextConfig.Option != DisableOffset) {
				pointTo = pointTo.Add(config.Offset);
			}
		}

		auto checkPos = current.Pos;
		return new Checkpoint(config.Type, drawPos, pointTo, config.Radius, config.Color, checkPos, config.BlipColor);
	}

	// Reset the mission.
	Checkpoint* Reset() {
		Current = 0;

		std::ostringstream s;
		s << "Training: ~b~" << Name << "~w~ (" << positions.size() << " waypoints)";
		Notify(s.str().c_str());

		return Advance();
	}

	std::string Name;
	int Current;

public:
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

void DebugPosition(Ped ped, Mission *current) {
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

	std::ostringstream stream;
	stream << "Mission: " << AUDIO::IS_MISSION_COMPLETE_PLAYING() << "flag: " << GAMEPLAY::GET_MISSION_FLAG() << " (" << name << ")";
	std::string s = stream.str();
	ShowSubtitle(s.c_str());
}

void ScriptMain() {
	bool debug = false;
	float ticker = 0.0;

	size_t missionIndex = 0;

	std::vector<Mission> missions;
	missions.push_back(FranklinAndLamar());
	missions.push_back(Chop());
	missions.push_back(Complications());

	Mission *current = &missions[missionIndex];
	Checkpoint* point = current->Advance();

	while (true) {
		if (IsKeyJustUp(VK_F4, true)) {
			debug = !debug;
		}

		if (IsKeyJustUp(VK_LEFT, true)) {
			if (missionIndex > 0) {
				missionIndex -= 1;
			}

			current = &missions[missionIndex];

			delete point;
			point = current->Reset();
		}

		if (IsKeyJustUp(VK_RIGHT, true)) {
			if (missionIndex + 1 < missions.size()) {
				missionIndex += 1;
				current = &missions[missionIndex];
				delete point;
				point = current->Reset();
			}
		}

		if (IsKeyJustUp(VK_DOWN, true)) {
			delete point;
			point = current->Decrement();
		}

		if (IsKeyJustUp(VK_UP, true)) {
			delete point;
			point = current->Advance();
		}

		ticker += GAMEPLAY::GET_FRAME_TIME();

		Player player = PLAYER::PLAYER_ID();
		Ped ped = PLAYER::GET_PLAYER_PED(player);

		Vec3 coords = ENTITY::GET_ENTITY_COORDS(ped, true);

		if (IsKeyJustUp(VK_DELETE, true)) {
			delete point;
			point = current->Advance();

			std::ofstream os;
			os.open("coords.txt", std::ofstream::out | std::ofstream::app);

			os << "points.Push({ Waypoint, {" << coords.x << ", " << coords.y << ", " << coords.z << "} });" << std::endl;
		}

		if (point != nullptr) {
			// ((x/2)^2 + (y/2)^2) < (z/2)^2 <==> (x^2 + y^2) / 4 < z/4
			if (point->CheckPos.Vdist2(coords) < point->Radius2 / 4) {
				delete point;
				point = current->Advance();
			}
		}

		// (x^2 + y^2) / 4 < z / 4

		if (debug) {
			if (ticker > 0.05) {
				DebugPosition(ped, current);
				ticker = 0;
			}
		}

		WAIT(0);
	}
}

Mission FranklinAndLamar() {
	Mission points("Franklin & Lamar");

	points.Push({ Waypoint, {-1886.48, -561.961, 11.2672}, {-2015.77, -455.14, 11.0581} });

	points.Push({ Waypoint, {-2015.77, -455.14, 11.0581}, {-1994, -419, 12} });

	points.Push({ Waypoint, {-1994, -419, 12}, {-1955, -424, 17} });
	points.Push({ Waypoint, {-1857, -487, 26}, {-1813, -517, 30} });

	// power point to prevent from going off the top of the hill.
	points.Push({ PowerPoint, {-1755.92, -550.803, 36.2118}, {-1733.77, -557.012, 36.7685} });
	points.Push({ Waypoint, {-1712, -563, 36}, {-1643, -597, 33} });
	points.Push({ BreakBoost, {-1637, -596, 33}, {-1635, -597, 33} });
	points.Push({ PowerPoint, {-1535, -642, 28}, {-1492, -631, 30} });
	points.Push({ BreakBoost, {-1479, -618, 30}, {-1474, -614.5, 30.5} });

	// intersection 1
	points.Push({ Waypoint, {-1316.15, -513.217, 32.6707}, {-1363, -550, 30} });
	// intersection 2
	points.Push({ Waypoint, {-1241, -448, 33}, {-1181, -420.5, 34} });
	// turn before studio
	points.Push({ Waypoint, {-1099, -414, 36}, {-1075, -434, 36} });
	// entrance to studio
	points.Push({ Waypoint, {-1055.55, -469.124, 35.8914}, {-1061.56, -483.848, 36.102} });
	// inside studio
	points.Push({ Waypoint, {-1113.38, -512.257, 34.2677}, {-1133.38, -522.442, 32.8011} });
	// exit to studio
	points.Push({ Waypoint, {-1207.22, -580.16, 26.6866}, {-1198.44, -633.651, 23.7369} });
	// after studio
	points.Push({ Waypoint, {-1173.97, -666.67, 22.5446}, {-1143.39, -700.038, 20.895}, 30 });
	// turn towards truck
	points.Push({ Waypoint, {-1108.82, -761.182, 18.7327}, {-1132.03, -813.194, 15.3551} });
	// turn down towards building site
	points.Push({ Waypoint, {-1157.63, -849.399, 13.7151}, {-1151.35, -876.797, 10.5292} });
	// first jump
	points.Push({ Waypoint, {-1090.76, -979.072, 3.9196}, {-1037.43, -1071.6, 3.66611} });
	points.Push({ Waypoint, {-1037.43, -1071.6, 3.66611}, {-979.515, -1162.63, 3.63968} });
	points.Push({ PowerPoint, {-979.515, -1162.63, 3.63968}, {-946.05, -1194.59, 4.64777} });
	// turn up towards vespucci road.
	points.Push({ PowerPoint, {-946.381, -1197.46, 4.65006}, {-907.4, -1188.37, 4.62585} });
	// intersection
	points.Push({ Waypoint, {-797.602, -1122.58, 10.2477}, {-762.629, -1104.09, 10.3714} });
	// hill up
	points.Push({ Waypoint, {-679.172, -1049.98, 15.7689}, {-659.784, -1026.22, 17.745} });
	// turn towards highway
	points.Push({ PowerPoint, {-619.136, -877.421, 24.6516}, {-597.697, -856.391, 25.2748} });

	points.Push({ Waypoint, {-518.104, -837.862, 30.0275}, {-482.834, -843.755, 30.0885}, 30 });

	points.Push({ PowerPoint, {-295.956, -835.602, 31.3429}, {-266.187, -797.451, 31.7652} });

	points.Push({ Waypoint, {-222.812, -695.03, 33.1329}, {-183.073, -679.134, 33.8081} });
	points.Push({ Waypoint, {-86.6128, -677.583, 34.8648}, {-75.0701, -681.3, 33.5304} });

	points.Push({ PowerPoint, {63.1746, -625.133, 31.2793}, {-54.6064, -756, 31.257} });
	points.Push({ Waypoint, {54.6064, -756.0, 31.257}, {135.0, -808.832, 30.8428} });

	points.Push({ Waypoint, {155.112, -831.111, 30.6528}, {108.104, -979.07, 28.9833} });

	points.Push({ Waypoint, {108.104, -979.07, 28.9833}, {71.3636, -1053.01, 28.9159} });
	// pointing into the alley.
	points.Push({ Waypoint, {71.3636, -1053.01, 28.9159}, {-32.0474, -1020.51, 28.439} });
	// down the alley with the cat.
	points.Push({ Waypoint, {-32.0474, -1020.51, 28.439}, {-51.1589, -1076.17, 26.518} });

	// Simeon's
	points.Push({ End, {-51.1589, -1076.17, 26.518}, {0, 0, 0} });
	return points;
}

Mission Chop() {
	Mission points("Chop");

	points.Push({ Marker, {-86.3744, -1443.55, 31.9378} });
	points.Push({ Waypoint, {-90.3165, -1421.71, 29.6781} });
	points.Push({ Waypoint, {-93.4267, -1376.57, 29.4077} });
	points.Push({ Waypoint, {-93.9633, -1249.6, 29.2218} });
	points.Push({ Waypoint, {-76.0679, -1120.89, 25.7841} });

	points.Push({ Waypoint, {-39.3668, -991.898, 29.2975} });
	points.Push({ BreakBoost, {32.5397, -803.768, 44.0959} });

	points.Push({ Waypoint, {63.4133, -723.15, 44.1713} });
	points.Push({ Waypoint, {110.176, -584.022, 43.831} });
	points.Push({ BreakBoost, {144.203, -478.993, 43.1441}, 15 });
	points.Push({ Waypoint, {201.386, -315.05, 44.0975} });
	points.Push({ BreakBoost, {224.512, -252.047, 53.3877} });
	points.Push({ BreakBoost, {273.695, -116.878, 69.4029} });

	points.Push({ Waypoint, {321.181, 12.116, 83.3376} });
	points.Push({ Waypoint, {354.845, 116.8, 103.115} });
	points.Push({ Waypoint, {389.548, 266.226, 103.126} });
	points.Push({ Waypoint, {376.788, 298.567, 103.41} });
	points.Push({ Waypoint, {364.01, 322.52, 103.719} });
	points.Push({ Waypoint, {370.469, 298.303, 103.384} });
	points.Push({ Waypoint, {389.566, 259.73, 103.006} });
	points.Push({ Waypoint, {369.611, 160.91, 103.113} });
	points.Push({ Waypoint, {351.5, 108.666, 102.671} });
	points.Push({ Waypoint, {294.701, -60.1733, 70.1573} });

	points.Push({ BreakBoost, {272.968, -119.35, 69.1539} });

	points.Push({ Waypoint, {221.461, -247.031, 53.8524} });
	points.Push({ Waypoint, {178.903, -356.478, 44.0171} });
	points.Push({ Waypoint, {135.062, -475.316, 43.188} });
	points.Push({ Waypoint, {131.184, -571.643, 43.7482} });
	points.Push({ Waypoint, {220.897, -626.298, 40.3986} });
	points.Push({ Waypoint, {253.868, -665.847, 38.2044} });
	points.Push({ Waypoint, {322.794, -690.317, 29.416} });
	points.Push({ Waypoint, {371.944, -677.177, 29.1781} });
	points.Push({ End, {439.964, -679.971, 28.8197} });

	// after the train section.
	points.Push({ Waypoint, {512.415, -653.313, 24.7689} });
	points.Push({ Waypoint, {499.688, -671.609, 25.1364} });
	points.Push({ Waypoint, {411.544, -669.429, 29.2909}, 15 });
	points.Push({ Waypoint, {338.449, -690.072, 29.3429}, 15 });
	points.Push({ Waypoint, {308.397, -801.908, 29.3238}, 15 });
	points.Push({ Waypoint, {274.776, -894.401, 28.9714}, 15 });
	points.Push({ Waypoint, {226.73, -1028.19, 29.3536}, 15 });
	points.Push({ Waypoint, {205.562, -1154.33, 29.3386}, 15 });
	points.Push({ Waypoint, {210.899, -1275.68, 29.3188}, 15 });
	points.Push({ Waypoint, {152.76, -1379.86, 29.2687}, 15 });
	points.Push({ Waypoint, {67.648, -1364.61, 29.3298}, 15 });
	points.Push({ Waypoint, {-86.3631, -1364.36, 29.4134}, 15 });
	points.Push({ Waypoint, {-205.998, -1440.22, 31.5089} });
	points.Push({ End, {-219.411, -1491.76, 31.283} });
	return points;
}

Mission Complications() {
	Mission points("Complications");

	points.Push({ Waypoint, {-228.266, -1492.31, 31.7989} });
	points.Push({ Waypoint, {-205.149, -1437.66, 31.1872} });
	points.Push({ Waypoint, {-122.303, -1354.72, 28.9651} });
	points.Push({ Waypoint, {-98.998, -1251.37, 28.9988} });
	points.Push({ Waypoint, {-79.583, -1155.07, 25.4651} });
	points.Push({ Waypoint, {-45.2943, -1126.07, 25.8511} });
	points.Push({ Waypoint, {-64.5105, -1093.21, 26.1703} });
	points.Push({ Waypoint, {-51.454, -960.192, 28.8842} });

	points.Push({ PowerPoint, {-58.4263, -936.818, 29.0519} });

	points.Push({ Waypoint, {-86.409, -921.105, 28.9834} });
	points.Push({ Waypoint, {-170.448, -894.406, 28.9708} });

	// first breakboost by the red garage.
	points.Push({ BreakBoost, {-281.374, -859.494, 31.4456}, 20 });
	points.Push({ Waypoint, {-363.681, -844.135, 31.2647}, 20 });	points.Push({ Waypoint, {-525.022, -838.268, 29.8906} });
	points.Push({ PowerPoint, {-616.351, -813.113, 25.0629} });
	points.Push({ BreakBoost, {-637.414, -678.306, 31.1733}, 20 });
	points.Push({ BreakBoost, {-634.434, -567.606, 34.725}, 30 });	points.Push({ Waypoint, {-633.324, -392.008, 34.4353}, 30 });
	points.Push({ Waypoint, {-654.292, -320.962, 34.6507}, 15 });	points.Push({ Waypoint, {-692.244, -255.339, 36.2569}, 15 });
	points.Push({ Waypoint, {-717.589, -183.392, 36.6689}, 15 });
	points.Push({ Waypoint, {-771.397, -88.8996, 37.3528} });
	points.Push({ Waypoint, {-828.837, 24.0653, 43.692} });	points.Push({ Waypoint, {-845.844, 69.9423, 51.5809} });
	points.Push({ Waypoint, {-861.394, 137.164, 59.8453} });	points.Push({ End, {-848.107, 158.031, 65.7154} });
	// house waypoints
	points.Push({ Waypoint, {-845.197, 163.113, 69.8114}, 3, DisableOffset });	points.Push({ Waypoint, {-826.444, 159.075, 69.4019}, 3, DisableOffset });	points.Push({ Waypoint, {-799.688, 165.224, 73.485}, 3, DisableOffset });
	points.Push({ End, {-802.198, 167.426, 76.7386}, 3, DisableOffset });
	// after picking up michael
	points.Push({ Waypoint, {-848.456, 158.604, 65.9644} });
	points.Push({ BreakBoost, {-862.5, 122.172, 57.1015} });
	points.Push({ Waypoint, {-852.574, 51.2438, 49.9765} });
	points.Push({ Waypoint, {-831.297, 4.7215, 42.4164} });
	points.Push({ Waypoint, {-789.568, -86.7867, 37.6706} });
	points.Push({ BreakBoost, {-766.89, -127.284, 37.8599} });
	points.Push({ BreakBoost, {-708.403, -229.143, 36.8906} });
	points.Push({ Waypoint, {-656.19, -319.957, 34.9128}, 15 });
	points.Push({ Waypoint, {-634.17, -403.567, 34.6915}, 30 });
	points.Push({ Waypoint, {-642.905, -573.716, 34.9605} });
	points.Push({ Waypoint, {-643.87, -749.315, 26.4412} });	// turn back towards red garage
	points.Push({ Waypoint, {-626.241, -802.352, 24.9278} });
	points.Push({ Waypoint, {-544.097, -842.187, 28.728} });
	points.Push({ Waypoint, {-459.495, -842.982, 30.3002}, 30 });
	points.Push({ BreakBoost, {-272.966, -863.996, 31.5794}, 30 });
	// turn towards Simeon's.
	points.Push({ Waypoint, {-61.5287, -932.35, 29.2371}, 20 });
	points.Push({ Waypoint, {-41.0718, -970.043, 29.2939}, 20 });
	points.Push({ Waypoint, {-77.8756, -1042.89, 27.8428}, 20 });
	points.Push({ End, {-97.732, -1090.37, 26.1147} });

	return points;
}