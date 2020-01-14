#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <vector>

// Team IDs
int kImmuneSystem = 0;
int kInfection    = 1;

// Bit flags for attack types
int kFire        = 1;
int kCold        = 2;
int kBludgeoning = 4;
int kSlashing    = 8;
int kRadiation   = 16;

// Constants to indicate no weakness/immunity (otherwise, it's a bitwise-or of attack types).
int kNoWeakness = 0;
int kNoImmunity = 0;


struct Group {
  int team;       // Which team this group belongs to, either kImmuneSystem or kInfection.
  int id;         // ID for this group.
  int count;      // Number of units remaining in this group. Gets depleted over the course of a battle.
  int hp;         // How many hit points each unit in this group has.
  int immune_to;  // Bit set for things this group is immune to
  int weak_to;    // Bit set for things this group is weak to
  int damage;     // How much damage each unit in this group does.
  int type;       // What type of damage attacks by this group do. This is exactly one of the attack type flags.
  int initiative; //

  Group* target = nullptr;
  Group* targetted_by = nullptr;

  int effective_power() const {
    return count * damage;
  }
};


bool target_selection_order(const Group* lhs, const Group* rhs) {
  int lhs_pow = lhs->effective_power();
  int rhs_pow = rhs->effective_power();
  return (lhs_pow != rhs_pow) ? (lhs_pow > rhs_pow) : lhs->initiative > rhs->initiative;
}


bool attack_order(const Group* lhs, const Group* rhs) {
  return lhs->initiative > rhs->initiative;
}


void remove_the_bodies(std::vector<Group*>& team)
{
  unsigned int i = 0;
  while (i < team.size()) {
    if (team[i]->count > 0) {
      i++;
      continue;
    }

    Group* body = team[i];
    if (body != team.back())
      team[i] = team.back();
    team.pop_back();

    delete body;
  }
}


void print_team(const char* name, const std::vector<Group*>& team)
{
  printf("%s:\n", name);
  for (const Group* g : team) {
    printf("Group %d contains %d units\n", g->id, g->count);
  }
}


void print_target_summary(const char* name, const std::vector<Group*>& team)
{
  for (const Group* g : team) {
    if (g->target == nullptr)
      continue;
    int damage = g->effective_power();
    if (g->target->weak_to & g->type)
      damage *= 2;
    printf("%s group %d would deal defending group %d %d damage\n", name, g->id, g->target->id, damage);
  }
}


void fight(std::vector<Group*>& immune_system, std::vector<Group*>& infection)
{
  // print_team("Immune System", immune_system);
  // print_team("Infection", infection);
  // printf("\n");

  const int kImmuneSystemSize = int(immune_system.size());
  const int kInfectionSize = int(infection.size());

  // Target selection
  std::vector<Group*> order(immune_system.size() + infection.size(), nullptr);
  for (int i = 0; i < kImmuneSystemSize; i++) {
    order[i] = immune_system[i];
  }
  for (int i = 0; i < kInfectionSize; i++) {
    order[i + kImmuneSystemSize] = infection[i];
  }
  std::sort(order.begin(), order.end(), target_selection_order);

  for (Group* g : order) {
    g->target = nullptr;
    g->targetted_by = nullptr;
  }

  for (Group* attacker : order) {
    std::vector<Group*>& potential_victims = (attacker->team == kImmuneSystem) ? infection : immune_system;
    int damage_to_target = 0;
    for (Group* victim : potential_victims) {
      if (victim->targetted_by != nullptr || (victim->immune_to & attacker->type))
        continue;

      int damage = attacker->effective_power();
      if (victim->weak_to & attacker->type) {
        damage *= 2;
      }
      if (damage > damage_to_target) {
        attacker->target = victim;
        damage_to_target = damage;
      }
      else if (damage == damage_to_target && victim->effective_power() > attacker->target->effective_power()) {
          attacker->target = victim;
      }
    }

    if (attacker->target != nullptr)
      attacker->target->targetted_by = attacker;
  }

  // print_target_summary("Infection", infection);
  // print_target_summary("Immune System", immune_system);
  // printf("\n");

  // Attacking
  std::sort(order.begin(), order.end(), attack_order);
  for (Group* attacker : order) {
    if (attacker->count <= 0 || attacker->target == nullptr || attacker->target->count <= 0)
      continue;
    int damage = attacker->effective_power();
    if (attacker->target->weak_to & attacker->type)
      damage *= 2;
    int killed = damage / attacker->target->hp;
    attacker->target->count -= killed;

    // printf("%s group %d attacks defending group %d, killing %d units\n",
    //        attacker->team == kImmuneSystem ? "Immune System" : "Infection",
    //        attacker->id, attacker->target->id, killed);
  }
  // printf("\n");
  // printf("\n");

  // The aftermath...
  remove_the_bodies(immune_system);
  remove_the_bodies(infection);
}


int main(int argc, char** argv)
{
  std::vector<Group*> immune_system, infection;
#if 0
  printf("Using example input\n");
  // Example input
  immune_system.push_back(new Group{ kImmuneSystem, 1, 17,  5390, kNoImmunity, kRadiation | kBludgeoning, 4507, kFire,     2 });
  immune_system.push_back(new Group{ kImmuneSystem, 2, 989, 1274, kFire,       kBludgeoning | kSlashing,  25,   kSlashing, 3 });

  infection.push_back(new Group{ kInfection, 1, 801,  4706, kNoImmunity, kRadiation,    116, kBludgeoning, 1 });
  infection.push_back(new Group{ kInfection, 2, 4485, 2961, kRadiation,  kFire | kCold, 12,  kSlashing,    4 });
#else
  printf("Using real input\n");
  // Real input

  // Immune System:
  immune_system.push_back(new Group{ kImmuneSystem,  1, 479,  3393,  kNoImmunity,                    kRadiation,           66,  kCold,        8  });
  immune_system.push_back(new Group{ kImmuneSystem,  2, 2202, 4950,  kSlashing,                      kFire,                18,  kCold,        2  });
  immune_system.push_back(new Group{ kImmuneSystem,  3, 8132, 9680,  kSlashing,                      kBludgeoning | kFire, 9,   kRadiation,   7  });
  immune_system.push_back(new Group{ kImmuneSystem,  4, 389,  13983, kBludgeoning,                   kNoWeakness,          256, kCold,        13 });
  immune_system.push_back(new Group{ kImmuneSystem,  5, 1827, 5107,  kNoImmunity,                    kNoWeakness,          24,  kSlashing,    18 });
  immune_system.push_back(new Group{ kImmuneSystem,  6, 7019, 2261,  kRadiation | kSlashing | kCold, kNoWeakness,          3,   kFire,        16 });
  immune_system.push_back(new Group{ kImmuneSystem,  7, 4736, 8421,  kNoImmunity,                    kCold,                17,  kSlashing,    3  });
  immune_system.push_back(new Group{ kImmuneSystem,  8, 491,  3518,  kFire | kBludgeoning,           kCold,                65,  kRadiation,   1  });
  immune_system.push_back(new Group{ kImmuneSystem,  9, 2309, 7353,  kRadiation,                     kNoWeakness,          31,  kBludgeoning, 20 });
  immune_system.push_back(new Group{ kImmuneSystem, 10, 411,  6375,  kSlashing,                      kCold | kFire,        151, kBludgeoning, 14 });

  // Infection:
  infection.push_back(new Group{ kInfection,  1, 148,  31914, kRadiation | kCold | kFire, kBludgeoning,      416, kCold,        4  });
  infection.push_back(new Group{ kInfection,  2, 864,  38189, kNoImmunity,                kNoWeakness,       72,  kSlashing,    6  });
  infection.push_back(new Group{ kInfection,  3, 2981, 7774,  kBludgeoning | kCold,       kNoWeakness,       4,   kFire,        15 });
  infection.push_back(new Group{ kInfection,  4, 5259, 22892, kNoImmunity,                kNoWeakness,       8,   kFire,        5  });
  infection.push_back(new Group{ kInfection,  5, 318,  16979, kNoImmunity,                kFire,             106, kBludgeoning, 9  });
  infection.push_back(new Group{ kInfection,  6, 5017, 32175, kRadiation,                 kSlashing,         11,  kBludgeoning, 17 });
  infection.push_back(new Group{ kInfection,  7, 4308, 14994, kFire | kCold,              kSlashing,         5,   kFire,        10 });
  infection.push_back(new Group{ kInfection,  8, 208,  14322, kNoImmunity,                kRadiation,        133, kCold,        19 });
  infection.push_back(new Group{ kInfection,  9, 3999, 48994, kNoImmunity,                kCold | kSlashing, 20,  kCold,        11 });
  infection.push_back(new Group{ kInfection, 10, 1922, 34406, kNoImmunity,                kSlashing,         35,  kSlashing,    12 });
#endif

  // Apply boost to immune system
  for (Group* g : immune_system) {
    g->damage += 37;
  }

  while (!immune_system.empty() && !infection.empty())
//  for (int i = 0; i < 3; i++)
    fight(immune_system, infection);

  int units_remaining = 0;
  if (!immune_system.empty()) {
    printf("Immune system won!\n");
    for (const Group* g : immune_system) {
      units_remaining += g->count;
    }
  }
  if (!infection.empty()) {
    printf("Infection won!\n");
    for (const Group* g : infection) {
      units_remaining += g->count;
    }
  }
  printf("%d units remaining\n", units_remaining);
}
