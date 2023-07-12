
#pragma once
// *** GENERATED FILE ***
// Generated from atlas.json

enum ImageIndex
{
  kElectric,
  kEnemy00,
  kEnemy01,
  kEnemy02,
  kEnemy03,
  kEnemy04,
  kEnemy05,
  kEnemy06,
  kEnemyAmmo00,
  kEnemyAmmo01,
  kEnemyAmmo02,
  kEnemyAmmo03,
  kEnemyAmmo04,
  kEnemyAmmoExplode00,
  kEnemyAmmoExplode01,
  kEnemyAmmoExplode02,
  kEnemyAmmoExplode03,
  kEnemyAmmoExplode04,
  kEnemyExtra01,
  kEnemyExtra03,
  kGlitter,
  kGroundBaseSea,
  kGroundMetalBase00,
  kGroundMetalBase01,
  kGroundMetalBase02,
  kGroundMetalBlip,
  kHero,
  kHeroAmmo00,
  kHeroAmmo01,
  kHeroAmmo02,
  kHeroAmmoExplode00,
  kHeroAmmoExplode01,
  kHeroAmmoExplode02,
  kHeroAmmoFlash00,
  kHeroAmmoFlash01,
  kHeroAmmoFlash02,
  kHeroShields,
  kHeroSuper,
  kLife,
  kMenuBack,
  kMenuUpDown,
  kPowerBurst,
  kPowerUpAmmo,
  kPowerUpShield,
  kReflect,
  kShields,
  kShipExplode,
  kStatBar,
  kStatTop,
  kSuperBomb,
  kUseFocus,
  kUseItem
};

extern float  g_ImageTexCoords[][4];
extern GLuint g_AtlasTexture;

void AtlasDrawSprite( int image_index, float pos_x, float pos_y, float scale_x = 1.0f, float scale_y = 1.0f );
