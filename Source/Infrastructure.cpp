#pragma once
#include "Infrastructure.h"

//PotentialField
double Cerebrate::Infrastructure::PotentialField::wpatch = 13000;
double Cerebrate::Infrastructure::PotentialField::wchoke = 1000;//1000;
double Cerebrate::Infrastructure::PotentialField::wpoly = 400;//1000;


double Cerebrate::Infrastructure::PotentialField::value(Intelligence::Agent const& a, const Builder builder, BWAPI::TilePosition tile, bool creep, bool occupied) {
	if (!a.graph)
		return 0;
	
	unsigned b = 0;
	for (; b < a.graph->bases.size(); b++)
		if (TerrainAnalysis::in(a.graph->bases[b].region,tile))
			break;

	if (b == a.graph->bases.size())
		return -80;
	
	std::set<Choke> chokes = a.graph->bases[b].base->getRegion()->getChokepoints();

	double tileValue = 0, chokeValue, patchValue, polyValue, d;

	TerrainAnalysis::Point set[4];
	set[0] = TerrainAnalysis::Point(tile);
	set[1] = set[0];
	set[1].x += 31;
	set[2] = set[0];
	set[2].y += 31;
	set[3] = set[2];
	set[3].x += 31;
	
	if (!BWAPI::Broodwar->isBuildable(tile,true))
		tileValue -= 80;

	for (unsigned k = 0; k < a.graph->bases[b].patches.size(); k++) {
		d = 0;
		for (unsigned j = 0; j < 4; j++)
			d += set[j].distance(BWAPI::Position(a.graph->bases[b].patches[k].position.first, a.graph->bases[b].patches[k].position.second));
		d /= 4;

		tileValue -= wpatch/(d*d);
	}

	for (unsigned k = 0; k < a.graph->bases[b].geysers.size(); k++) {
		d = 0;
		for (unsigned j = 0; j < 4; j++)
			d += set[j].distance(BWAPI::Position(a.graph->bases[b].geysers[k].position.first, a.graph->bases[b].geysers[k].position.second));
		d /= 4;

		tileValue -= 4*wpatch/(d*d);
	}

	for (std::set<Choke>::const_iterator choke = chokes.begin(); choke != chokes.end(); choke++) {
		d = 0;
		for (unsigned j = 0; j < 4; j++)
			d += set[j].distance((*choke)->getCenter());
		d /= 4;

		chokeValue = (*choke)->getWidth() < 80 ? (*choke)->getWidth() : 80;
		chokeValue *= (wchoke/(d*d));
		tileValue -= chokeValue;
	}

	d = 1e37;
	for (unsigned i = 0; i < a.graph->bases[b].region.size(); i++) {
		double dist = 0;
		for (unsigned j = 0; j < 4; j++)
			dist += a.graph->bases[b].region[i].distance(set[j]);
		dist /= 4;
		
		if (d > dist)
			d = dist;
	}

	if (d == 0)
		return 80;
	
	polyValue = wpoly/(d*d);
	tileValue += polyValue;

	if (tileValue > 80)
		return 80;
	if (tileValue < -80)
		return -80;
	
	if (occupied && builder.isOccupied(tile))
		tileValue -= 80;
	
	if (creep && !BWAPI::Broodwar->hasCreep(tile))
		tileValue -= 160;
	
	return tileValue;
}

double Cerebrate::Infrastructure::PotentialField::value(Intelligence::Agent const& a, const Builder b, int x, int y, BWAPI::TilePosition position, bool creep, bool occupied) {
	double ret = 0;
		for (int i = 0; i < x; i++)
			for (int j = 0; j < y; j++) {
				BWAPI::TilePosition new_pos(position.x()+i, position.y()+j);
				ret += value(a,b,new_pos,creep,occupied);
			}
	
	return ret;
}

double Cerebrate::Infrastructure::PotentialField::valueForTarget(BWAPI::Position pos, Intelligence::Agent const& a, const Builder builder, BWAPI::TilePosition tile, bool creep, bool occupied) {
	double d, tileValue = 0;
	
	TerrainAnalysis::Point set[4];
	set[0] = TerrainAnalysis::Point(tile);
	set[1] = set[0];
	set[1].x += 31;
	set[2] = set[0];
	set[2].y += 31;
	set[3] = set[2];
	set[3].x += 31;
	
	unsigned b = 0;
	for (; b < a.graph->bases.size(); b++)
		if (TerrainAnalysis::in(a.graph->bases[b].region,tile))
			break;

	if (b == a.graph->bases.size())
		return -80;
	
	for (unsigned k = 0; k < a.graph->bases[b].patches.size(); k++) {
		d = 0;
		for (unsigned j = 0; j < 4; j++)
			d += set[j].distance(BWAPI::Position(a.graph->bases[b].patches[k].position.first, a.graph->bases[b].patches[k].position.second));
		d /= 4;

		tileValue -= wpatch/(d*d);
	}

	for (unsigned k = 0; k < a.graph->bases[b].geysers.size(); k++) {
		d = 0;
		for (unsigned j = 0; j < 4; j++)
			d += set[j].distance(BWAPI::Position(a.graph->bases[b].geysers[k].position.first, a.graph->bases[b].geysers[k].position.second));
		d /= 4;

		tileValue -= 4*wpatch/(d*d);
	}
	
	d = 0;
	for (unsigned j = 0; j < 4; j++)
		d += set[j].distance(pos);
	d /= 4;
	
	tileValue += 100000/(d*d);
	
	if (!BWAPI::Broodwar->isBuildable(tile,true))
		tileValue -= 80;
	if (occupied && builder.isOccupied(tile))
		tileValue -= 80;	
	if (creep && !BWAPI::Broodwar->hasCreep(tile))
		tileValue -= 160;
	
	return tileValue;
}
double Cerebrate::Infrastructure::PotentialField::valueForTarget(BWAPI::Position pos, Intelligence::Agent const& a, const Builder b, int x, int y, BWAPI::TilePosition tile, bool creep, bool occupied) {
	double ret = 0;
		for (int i = 0; i < x; i++)
			for (int j = 0; j < y; j++) {
				BWAPI::TilePosition new_pos(tile.x()+i, tile.y()+j);
				ret += valueForTarget(pos,a,b,new_pos,creep,occupied);
			}
	
	return ret;
}

bool Cerebrate::Infrastructure::PotentialField::onCreep(int x, int y, BWAPI::TilePosition position) {
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			if (BWAPI::Broodwar->hasCreep(position.x()+i,position.y()+j))
				return true;
	return false;
}

//// BuilderDrone
Cerebrate::Infrastructure::BuilderDrone::BuilderDrone(Unit d, BWAPI::UnitType b, BWAPI::TilePosition t, unsigned id)
: drone(d), building(b), state(Cerebrate::Infrastructure::Moving),target(t), budgetID(id) {
	center.x() = target.x()*32 + building.tileWidth()*16;
	center.y() = target.y()*32 + building.tileHeight()*16;
}

//// BuilderSet
void Cerebrate::Infrastructure::BuilderSet::act(Economy::Economist& eco, Industry::Manager& man, Resources::Miner& miner, Intelligence::Agent& a, Builder& b) {
	for (unsigned i = 0; i < builders.size(); i++)
		switch(builders[i].state) {
			case Cerebrate::Infrastructure::Moving:
				if (builders[i].drone->getPosition() != builders[i].center)
					builders[i].drone->move(builders[i].center);
				else
					builders[i].state = Cerebrate::Infrastructure::Building;
				break;
			case Cerebrate::Infrastructure::Building:
				if (builders[i].drone->isUnderAttack()) {
					if (builders[i].drone->isMorphing()) {
						builders[i].drone->cancelMorph();
						
						builders[i].budgetID = eco.add(builders[i].building);
					}
					builders[i].state = Cerebrate::Infrastructure::Fleeing;
				} else {
					if (builders[i].drone->getType() == Drone)
						builders[i].drone->build(builders[i].target,builders[i].building);
					else if (builders[i].drone->isBeingConstructed())
						eco.remove(builders[i].budgetID);
					else {
						builders[i].state = Cerebrate::Infrastructure::Done;
						if (builders[i].building == Hatch) {
							man.hatcheries.push_back(builders[i].drone);
							b.addHatch(builders[i].drone,a);
							miner.add(builders[i].drone);
							a.expanded(builders[i].drone->getPosition());
						}
					}	
				}
				break;
			case Cerebrate::Infrastructure::Fleeing:
				if (!builders[i].drone->isUnderAttack())
					builders[i].state = Cerebrate::Infrastructure::Moving;
				break;
		}
}

void Cerebrate::Infrastructure::BuilderSet::draw() const {
	for (unsigned i = 0; i < builders.size(); i++) {
		BWAPI::Color color;
		char* str;
		switch(builders[i].state) {
			case Cerebrate::Infrastructure::Moving:
				str = "\x11 Moving";
				color = BWAPI::Colors::Orange;
				break;
			case Cerebrate::Infrastructure::Building:
				str = "\x10 Building";
				color = BWAPI::Colors::Purple;
				break;
			case Cerebrate::Infrastructure::Fleeing:
				str = "\x17 Fleeing";
				color = BWAPI::Colors::Yellow;
				break;
			case Cerebrate::Infrastructure::Done:
				str = "\0";
				color = BWAPI::Colors::Brown;
				break;
		}
		
		BWAPI::Broodwar->drawTextMap(builders[i].drone->getPosition().x()-12, builders[i].drone->getPosition().y()-4, str);
		BWAPI::Broodwar->drawLineMap(builders[i].drone->getPosition().x(), builders[i].drone->getPosition().y(),
									 builders[i].center.x(), builders[i].center.y(), color);
		BWAPI::Broodwar->drawBoxMap(builders[i].target.x()*32, builders[i].target.y()*32,
									builders[i].target.x()*32+builders[i].building.tileWidth()*32,
									builders[i].target.y()*32+builders[i].building.tileHeight()*32, color);
		
	}
}

bool Cerebrate::Infrastructure::BuilderSet::in(Unit d) const {
	for(unsigned i = 0; i < builders.size(); i++)
		if (builders[i].drone == d)
			return true;
	return false;
}

//// Hatchery

bool Cerebrate::Infrastructure::Hatchery::isOccupied(BWAPI::TilePosition pos) const {
	if (!BWAPI::Broodwar->isBuildable(pos,true))
		return false;
	for(unsigned i = 0; i < wall.size(); i++)
		if (wall[i].isOccupied(pos))
			return true;
	for(unsigned i = 0; i < sunkens.size(); i++)
		if (sunkens[i].isOccupied(pos))
			return true;
	for(unsigned i = 0; i < spores.size(); i++)
		if (spores[i].isOccupied(pos))
			return true;
	return false;
}
bool Cerebrate::Infrastructure::Hatchery::canWall() const {
	if (!base)
		return false;
	if (base->base->isStartLocation())
		return false;
	if (base->base->getRegion()->getChokepoints().size() < 2)
		return false;
	
	std::set<Choke> chokes = base->base->getRegion()->getChokepoints();
	std::vector<BWTA::Region*> adjacents;

	for(std::set<Choke>::const_iterator choke = chokes.begin(); choke != chokes.end(); choke++) {
		unsigned i = 0;
		if ((*choke)->getRegions().first == base->base->getRegion()) {
			for (; i < adjacents.size(); i++)
				if (adjacents[i] == (*choke)->getRegions().second)
					break;
			if (i == adjacents.size())
				adjacents.push_back((*choke)->getRegions().second);
		} else {
			for (; i < adjacents.size(); i++)
				if (adjacents[i] == (*choke)->getRegions().first)
					break;
			if (i == adjacents.size())
				adjacents.push_back((*choke)->getRegions().first);
		}		
	}
	
	return adjacents.size() < 3;
}
bool Cerebrate::Infrastructure::Hatchery::adjacent(BuildingSlot s) const {
	if (s.position.y() == hatch.y() + 3 || s.position.y() + s.y - 1 == hatch.y() - 1)
		for (int i = 0; i < s.x; i++)
			if (s.position.x() + i >= hatch.x() - 1 && s.position.x() + i <= hatch.x() + 4)
				return true;
	
	if (s.position.x() == hatch.x() + 4 || s.position.x() + s.x - 1 == hatch.x() - 1)
		for (int i = 0; i < s.y; i++)
			if (s.position.y() + i >= hatch.y() - 1 && s.position.y() + i <= hatch.y() + 3)
				return true;
	
		
	return false;
}

//// Builder

void Cerebrate::Infrastructure::Builder::addHatch(Unit hatch, Intelligence::Agent& a) {
	Intelligence::BaseInfo* base = 0;
	
	if (a.graph)
		for (unsigned i = 0; i < a.graph->bases.size(); i++)
			if (a.graph->bases[i].base->getPosition() == hatch->getPosition()) {
				a.graph->bases[i].owner = Intelligence::Mine;
				base = &(a.graph->bases[i]);
				break;
			}
	
	hatcheries.insert(hatcheries.end(),Hatchery(hatch,base));
	
	unsigned h = hatcheries.size()-1;
	if (hatcheries[h].canWall() && !hatcheries[h].wall.size()) {
		BuildingSlot macro;
		macro.x = 4;
		macro.y = 3;
		
		spiral(a,BWAPI::Position(4,3),macro,hatcheries[h].hatch,false);
		
		hatcheries[h].wall.push_back(macro);
		
		while(!hatcheries[h].adjacent(hatcheries[h].wall.back())) {
			double value = -1e37;
			
			BuildingSlot building;
			building.x = 2;
			building.y = 2;
			//2x2
			for (int i = -building.x; i <= hatcheries[h].wall.back().x; i++) {
				BWAPI::TilePosition tile = BWAPI::TilePosition(hatcheries[h].wall.back().position.x()+i,hatcheries[h].wall.back().position.y()-building.y);
				double v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building.x,building.y, tile, false);
				
				if (v > value) {
					value = v;
					building.position = tile;
				}
				
				tile.y() = hatcheries[h].wall.back().position.y() + hatcheries[h].wall.back().y;
				v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building.x,building.y, tile, false);
				
				if (v > value) {
					value = v;
					building.position = tile;
				}
			}
			
			for (int i = 0; i < hatcheries[h].wall.back().y; i++) {
				BWAPI::TilePosition tile = BWAPI::TilePosition(hatcheries[h].wall.back().position.x()-building.x,hatcheries[h].wall.back().position.y()+i);
				double v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building.x,building.y, tile, false);
				
				if (v > value) {
					value = v;
					building.position = tile;
				}
				
				tile.x() = hatcheries[h].wall.back().position.x() + hatcheries[h].wall.back().x;
				v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building.x,building.y, tile, false);
				
				if (v > value) {
					value = v;
					building.position = tile;
				}
			}
			
			if (hatcheries[h].adjacent(building)) {
				hatcheries[h].wall.push_back(building);
				break;
			}
			
			//3x2
			double value3 = -1e37;
			
			BuildingSlot building3;
			building3.x = 3;
			building3.y = 2;
			
			for (int i = -building3.x; i <= hatcheries[h].wall.back().x; i++) {
				BWAPI::TilePosition tile = BWAPI::TilePosition(hatcheries[h].wall.back().position.x()+i,hatcheries[h].wall.back().position.y()-building.y);
				double v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building3.x,building3.y, tile, false);
				
				if (v > value3) {
					value3 = v;
					building3.position = tile;
				}
				
				tile.y() = hatcheries[h].wall.back().position.y() + hatcheries[h].wall.back().y;
				v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building3.x,building3.y, tile, false);
				
				if (v > value3) {
					value3 = v;
					building3.position = tile;
				}
			}
			
			for (int i = 0; i < hatcheries[h].wall.back().y; i++) {
				BWAPI::TilePosition tile = BWAPI::TilePosition(hatcheries[h].wall.back().position.x()-building.x,hatcheries[h].wall.back().position.y()+i);
				double v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building3.x,building3.y, tile, false);
				
				if (v > value3) {
					value3 = v;
					building3.position = tile;
				}
				
				tile.x() = hatcheries[h].wall.back().position.x() + hatcheries[h].wall.back().x;
				v = PotentialField::valueForTarget(hatcheries[h].base->base->getPosition(),a,*this, building3.x,building3.y, tile, false);
				
				if (v > value3) {
					value3 = v;
					building3.position = tile;
				}
			}
			
			if (value3 > value)
				hatcheries[h].wall.push_back(building3);
			else
				hatcheries[h].wall.push_back(building);
		}
	}
}
void Cerebrate::Infrastructure::Builder::draw(Intelligence::Agent const& a) const {
	for (unsigned i = 0; i < hatcheries.size(); i++) {
		BWAPI::Position pos(hatcheries[i].hatch);
		
		pos.x() += 63;
		pos.y() += 47;
		
		if (hatcheries[i].isMacro())
			BWAPI::Broodwar->drawTextMap(pos.x() - 10, pos.y(), "MACRO");
		else {
			if (hatcheries[i].base == a.graph->self().info)
				BWAPI::Broodwar->drawTextMap(pos.x() - 10, pos.y(), "MAIN");
			else if (hatcheries[i].base == a.graph->self().natural.info) {
				BWAPI::Broodwar->drawTextMap(pos.x() - 10, pos.y(), "NATURAL [%s]", (hatcheries[i].canWall() ? "YES" : "NO"));
				
				//for (int k = -10; k < 11; k++)
				//	for (int l = -10; l < 11; l++) {
				//		BWAPI::TilePosition pp(hatcheries[i].hatch);
				//		pp.x() += k;
				//		pp.y() += l;
				//		
				//		BWAPI::Position qq(pp);
				//		
				//		BWAPI::Broodwar->drawTextMap(qq.x(),qq.y(),"\x04%.0f\n\x11%.2f", PotentialField::value(a,*this,4,3,pp,false,false),PotentialField::value(a,*this,pp,false,false));
				//		/*BWAPI::Broodwar->drawTextMap(qq.x(),qq.y(),"\x04%.2f\n%dx%d",
				//			PotentialField::valueForTarget(hatcheries[i].base->base->getPosition(),a,*this,pp,false,true),
				//			pp.x(),pp.y());*/
				//	}
			} else
				BWAPI::Broodwar->drawTextMap(pos.x() - 10, pos.y(), "BASE [%s]", (hatcheries[i].canWall() ? "YES" : "NO"));
			
			for (unsigned j = 0; j < hatcheries[i].wall.size(); j++)
				BWAPI::Broodwar->drawBoxMap(hatcheries[i].wall[j].position.x()*32, hatcheries[i].wall[j].position.y()*32,
											hatcheries[i].wall[j].position.x()*32 + hatcheries[i].wall[j].x*32,
											hatcheries[i].wall[j].position.y()*32 + hatcheries[i].wall[j].y*32, BWAPI::Colors::Orange);
		}
	}

	builders.draw();
}
void Cerebrate::Infrastructure::Builder::act(Economy::Economist& eco, Industry::Manager& man, Resources::Miner& miner, Intelligence::Agent& a) {
	builders.act(eco,man,miner,a,*this);
}
void Cerebrate::Infrastructure::Builder::update(Industry::Manager& man, Intelligence::Agent& a) {
	for (unsigned i = 0; i < builders.builders.size(); i++)
		if (!builders.builders[i].drone->exists()) {
			man.add(Industry::Production(builders.builders[i].building, 0.91));
			builders.builders.erase(builders.builders.begin()+i);
			i--;
		}
	
	if (a.graph) {
		for (unsigned i = 0; i < hatcheries.size(); i++)
			if (!hatcheries[i].base)
				for (unsigned j = 0; j < a.graph->bases.size(); j++)
					if (hatcheries[i].hatch == a.graph->bases[j].base->getTilePosition()) {
						hatcheries[i].base = &a.graph->bases[j];
						a.graph->bases[j].owner = Intelligence::Mine;
						break;
					}
	}
}

bool Cerebrate::Infrastructure::Builder::isOccupied(BWAPI::TilePosition pos) const {
	for (unsigned i = 0; i < hatcheries.size(); i++)
		if (hatcheries[i].isOccupied(pos))
			return true;
	return false;
}

BWAPI::Position Cerebrate::Infrastructure::Builder::spiral(Intelligence::Agent const& a, BWAPI::Position center, Cerebrate::Infrastructure::BuildingSlot& slot, BWAPI::TilePosition position, bool creep) const {
	struct Position {
		int x;
		int y;
		Position(int i, int j):x(i),y(j) { }
		BWAPI::TilePosition tile() const { return BWAPI::TilePosition(x,y); }
		BWAPI::Position pos() const { return BWAPI::Position(tile()); }
	};
	
	Position current(position.x(),position.y()+center.y()), target(center.x(),center.y()+slot.y);

	struct Candidate {
		BWAPI::TilePosition position;
		double value;
		
		Candidate(BWAPI::TilePosition& pos, double v) : position(pos),value(v) { }
		
		bool operator<(Candidate b) const { return value < b.value; }
	};
	
	std::vector< Candidate > candidates;
	
	if (creep) {
		for (int i = 0; i <= target.x; i++) {
			current.x += i;
			candidates.push_back(Candidate(current.tile(), PotentialField::value(a, *this, slot.x,slot.y,current.tile(),creep)));
		}
		target.x += slot.x;
		
		for (int sign = 1; PotentialField::onCreep(slot.x,slot.y,current.tile()); sign = -sign, target.x++, target.y++) {
			for (int i = 0; i < target.y; i++) {
				current.y += i * sign;
				candidates.push_back(Candidate(current.tile(), PotentialField::value(a, *this, slot.x,slot.y,current.tile(),creep)));
			}

			for (int i = 0; i < target.x; i++) {
				current.x -= i * sign;
				candidates.push_back(Candidate(current.tile(), PotentialField::value(a, *this, slot.x,slot.y,current.tile(),creep)));
			}
		}
		
		std::sort(candidates.begin(), candidates.end());
		slot.position = candidates.back().position;
	} else {
		bool changed = true;
		slot.position = position;
		double current_value = PotentialField::value(a, *this, slot.x,slot.y,position,creep);
		
		for (int i = -slot.x; i <= center.x(); i++) {
			BWAPI::TilePosition tile = BWAPI::TilePosition(position.x()+i,position.y()-slot.y);
			double v = PotentialField::value(a,*this, slot.x,slot.y, tile, false);
			
			if (v > current_value) {
				current_value = v;
				slot.position = tile;
			}
			
			tile.y() = position.y() + slot.y;
			v = PotentialField::value(a,*this, slot.x,slot.y, tile, false);
			
			if (v > current_value) {
				current_value = v;
				slot.position = tile;
			}
		}
		
		for (int i = 0; i < slot.y; i++) {
			BWAPI::TilePosition tile = BWAPI::TilePosition(position.x()-slot.x,position.y()+i);
			double v = PotentialField::value(a,*this, slot.x,slot.y, tile, false);
			
			if (v > current_value) {
				current_value = v;
				slot.position = tile;
			}
			
			tile.x() = position.x() + slot.x;
			v = PotentialField::value(a,*this, slot.x,slot.y, tile, false);
			
			if (v > current_value) {
				current_value = v;
				slot.position = tile;
			}
		}
		
		while(changed) {
			for (int i = -1; i < 2; i++)
				for (int j = -1; j < 2; j++)
					if (i || j) {
						BWAPI::TilePosition new_pos(slot.position.x()+i, slot.position.y()+j);
						double value = PotentialField::value(a, *this, slot.x,slot.y,new_pos,creep);
						if (value > current_value)
							candidates.push_back(Candidate(new_pos,value));
					}
			std::sort(candidates.begin(),candidates.end());
			
			if (candidates.size()) {
				slot.position = candidates.back().position;
				current_value = candidates.back().value;
				candidates.clear();
			} else
				changed = false;
		}
	}
	return BWAPI::Position(slot.position);
}

bool Cerebrate::Infrastructure::Builder::build(Cerebrate::Resources::Miner& miner, Intelligence::Agent const& a, BWAPI::UnitType structure, unsigned id) {
	if (structure == Sunken)
		;
	else if (structure == Spore)
		;
	else {
		BuildingSlot s;
		s.x = structure.tileWidth();
		s.y = structure.tileHeight();
		s.position = BWAPI::TilePosition(0,0);
		
		for(unsigned i = 0; i < hatcheries.size(); i++)
			if (hatcheries[i].canWall()) {
				for (unsigned j = 0; j < hatcheries[i].wall.size(); j++)
					if (hatcheries[i].wall[j].x == s.x &&
						hatcheries[i].wall[j].y == s.y)
						s.position = hatcheries[i].wall[j].position;
			}
		
		if (!s.position.x() && !s.position.y()) {
			if (structure.isResourceDepot())
				s.position = a.nextBase();
			else
				spiral(a,BWAPI::Position(4,3),s,hatcheries[0].hatch, true);
		}
		
		Unit target = miner.getDrone(BWAPI::Position(s.position));
		if (target) {
			builders.builders.push_back(BuilderDrone(target,structure,s.position, id));
			return true;
		}
		return false;
	}
}