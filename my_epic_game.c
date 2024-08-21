typedef struct Sprite{
	Gfx_Image* image;
	Vector2 size;
}Sprite;

typedef enum SpriteID{
	SPRITE_nill,
	SPRITE_player,
	SPRITE_tree0,
	SPRITE_tree1,
	SPRITE_rock0,
	SPRITE_rock1,
	SPRITE_MAX,
}SpriteID;

Sprite sprites[SPRITE_MAX];

Sprite* get_sprite(SpriteID id){
	if(id >= 0 && id < SPRITE_MAX){
		return &sprites[id];
	}
	return &sprites[0];
}

typedef enum EntityArchetype {
	arch_nil = 0,
	arch_rock = 1,
	arch_tree = 2,
	arch_player = 3,
} EntityArchetype;

typedef struct Entity{
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;

	bool render_sprite;
	SpriteID sprite_id;
}Entity;
#define MAX_ENTITY_COUNT 1024

typedef struct World {
	Entity entities[MAX_ENTITY_COUNT];
}World;

World* world = 0;

Entity* entity_create(){
	Entity* entity_found = 0;
	for (int i = 0; i < MAX_ENTITY_COUNT; i++){
		Entity* existing_entity = &world->entities[i];
		if (!existing_entity->is_valid){
			entity_found = existing_entity;
			break;
		}
	}
	assert(entity_found,"No more entities!");
	entity_found->is_valid = true; 
	return entity_found;
}

void entity_destroyer(Entity* entity){
	memset(entity, 0, sizeof(Entity));
}

void setup_player(Entity* en){
	en->arch = arch_player;
	en->sprite_id = SPRITE_player;
}

void setup_rock(Entity* en){
	en->arch = arch_rock;
	en->sprite_id = SPRITE_rock0;
}

void setup_tree(Entity* en){
	en->arch = arch_tree;
	en->sprite_id = SPRITE_tree0;
	//en->sprite_id = SPRITE_tree1;
}



int entry(int argc, char **argv) {
	
	window.title = STR("My Epic Game");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 90;
	window.clear_color = hex_to_rgba(0x6495EDff);

	world = alloc(get_heap_allocator(), sizeof(World));

	sprites[SPRITE_player] = (Sprite){.image=load_image_from_disk(fixed_string("player0.png"), get_heap_allocator()), .size = v2(17.0 ,20.0)};
	sprites[SPRITE_tree0] = (Sprite){.image=load_image_from_disk(fixed_string("tree0.png"), get_heap_allocator()), .size = v2(23.0 ,52.0)};
	sprites[SPRITE_tree1] = (Sprite){.image=load_image_from_disk(fixed_string("tree1.png"), get_heap_allocator()), .size=v2(45,44)};
	sprites[SPRITE_rock0] = (Sprite){.image=load_image_from_disk(fixed_string("rock0.png"), get_heap_allocator()), .size = v2(43.0 ,20.0)};

	Entity* player_en = entity_create();
	setup_player(player_en);

	for (int i = 0; i < 10; i++){
		Entity* en = entity_create();
		setup_rock(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
	}

	for (int i = 0; i < 10; i++){
		Entity* en = entity_create();
		setup_tree(en);
		en->pos = v2(get_random_float32_in_range(-200, 200), get_random_float32_in_range(-200, 200));
	}
	//Vector2 player_en->pos = v2(0,0);
	float64 seconds_counter = 0.0;
	int frame_count = 0;

	float64 last_time = os_get_current_time_in_seconds();
	float scale_p = 0.5;
	float zoom = 3;
	while (!window.should_close) {
		reset_temporary_storage();

		draw_frame.projection = m4_make_orthographic_projection(window.width * -scale_p, window.width * scale_p, window.height * -scale_p, window.height * scale_p, -1, 10);

		draw_frame.view = m4_make_scale(v3(1.0/zoom ,1.0/zoom ,1.0/zoom));

		float64 now = os_get_current_time_in_seconds();
		float64 delta_t = now - last_time;
		last_time = now;

		os_update(); 

		for (int i = 0; i < MAX_ENTITY_COUNT; i++){
			Entity* en = &world->entities[i];
			if(en->is_valid){
				switch (en->arch){
								
					default:
						{
							Sprite* sprite = get_sprite(en->sprite_id);
							Matrix4 xform = m4_scalar(1.0);
							xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
							xform         = m4_translate(xform, v3(sprite->size.x * -0.5, 0.0, 0));
							draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
							
							//draw_rect(v2(sin(now), -.8), v2(.5, .25), COLOR_RED);
							break;
						}
				}
			}
		}

		if (is_key_just_pressed(KEY_ESCAPE)){
			window.should_close = true;
		}

		Vector2 input_axis = v2(0,0);
		if (is_key_down('A')){
			input_axis.x -= 1.0;
		}
		if (is_key_down('D')){
			input_axis.x += 1.0;
		}
		if (is_key_down('S')){
			input_axis.y -= 1.0;
		}
		if (is_key_down('W')){
			input_axis.y += 1.0;
		}
		input_axis = v2_normalize(input_axis);

		player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, 100.0*delta_t));

		gfx_update();
		seconds_counter += delta_t;
		frame_count += 1;
		if(seconds_counter > 1.0){
			log("fps: %i", frame_count);
			seconds_counter = 0.0;
			frame_count = 0;
		}
	}

	return 0;
}