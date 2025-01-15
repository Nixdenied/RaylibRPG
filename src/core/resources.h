typedef struct {
    int wood;
    int gold;
} Resources;


void DrawResources(const Resources *resources, int screenWidth);
void AddWood(Resources *resources, int amount);
void AddGold(Resources *resources, int amount);
void InitResources(Resources *resources);