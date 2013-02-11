typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//Light0 distant light outside
//float attenuation_constant_d, attenuation_linear_d, attenuation_quadratic_d;
point4 distant_light_position( 100.0, 100.0, 100.0, 0.0 );
color4 distant_light_ambient( 0.8, 0.8, 0.8, 1.0 );
color4 distant_light_diffuse( 1.0, 1.0, 1.0, 1.0 );
color4 distant_light_specular( 1.0, 1.0, 1.0, 1.0 );

//Light0 point light inside
point4 point_light_position( 0.0, 0.0, -20.0, 1.0 );
color4 point_light_ambient( 1.0, 1.0, 1.0, 1.0 );
color4 point_light_diffuse( 1.0, 1.0, 1.0, 1.0 );
color4 point_light_specular( 1.0, 1.0, 1.0, 1.0 );

//------------------------------------------------------------------------------------------
//Material0 ruby
color4 ruby_ambient( 0.174500, 0.011750, 0.011750, 1.0000 );
color4 ruby_diffuse( 0.614240, 0.041360, 0.041360, 1.0000 );
color4 ruby_specular( 0.727811, 0.626959, 0.626959, 1.0000 );
GLfloat ruby_shininness = 76.800003;

//Material1 jade
color4 jade_ambient( 0.135, 0.2225, 0.1575, 1 );
color4 jade_diffuse( 0.54, 0.89, 0.63, 1 );
color4 jade_specular( 0.316228, 0.316228, 0.316228, 1 );
GLfloat jade_shininness = 0.1;				

//Material2 emerald
color4 emerald_ambient( 0.0215, 0.1745, 0.0215, 1 );
color4 emerald_diffuse( 0.07568, 0.61424, 0.07568, 1 );
color4 emerald_specular( 0.633, 0.727811, 0.633, 1 );
GLfloat emerald_shininness = 0.6;	

//Material3 obsidian
color4 obsidian_ambient( 0.18275, 0.17, 0.22525, 1 );
color4 obsidian_diffuse( 0.18275, 0.17, 0.22525, 1 );
color4 obsidian_specular( 0.18275, 0.17, 0.22525, 1 );
GLfloat obsidian_shininness = 0.3;	

//Material4 green
color4 green_ambient( 0.0215, 0.1745, 0.05, 1 );
color4 green_diffuse( 0.07568, 0.61424, 0.07568, 1 );
color4 green_specular( 0.033, 0.05, 0.01, 1 );
GLfloat green_shininness = 0.178125;	

//Material5 gold
color4 gold_ambient( 0.247250, 0.224500, 0.064500, 1.000000 );
color4 gold_diffuse( 0.346150, 0.314300, 0.090300, 1.000000 );
color4 gold_specular( 0.797357, 0.723991, 0.208006, 1.000000 );
GLfloat  gold_shininness = 83.199997;

//Material6 white
color4 white_ambient( 0.5, 0.5, 0.5, 1 );
color4 white_diffuse( 0.85, 0.85, 0.85, 1 );
color4 white_specular( 0.90, 0.90, 0.90, 1 );
GLfloat white_shininness = 0.25;	

//Material7 yellow
color4 yellow_ambient( 0.95, 0.95, 0.0, 1 );
color4 yellow_diffuse( 0.5, 0.5, 0.4, 1 );
color4 yellow_specular( 0.7, 0.7, 0.04, 1 );
GLfloat yellow_shininness = 0.078125;	

//Material8 trunk
color4 trunk_ambient( 0.2125, 0.1275, 0.054, 1 );
color4 trunk_diffuse( 0.314, 0.2284, 0.18144, 1 );
color4 trunk_specular( 0.193548, 0.271906, 0.166721, 1 );
GLfloat trunk_shininness = 0.2;	
//------------------------------------------------------------------------------------------
//Products0 ground
color4 ground_a = distant_light_ambient * emerald_ambient;
color4 ground_d = distant_light_diffuse * emerald_diffuse;
color4 ground_s = distant_light_specular * emerald_specular;
GLfloat ground_sh = emerald_shininness;

//Products1 pillar
color4 pillar_a = distant_light_ambient * ruby_ambient;
color4 pillar_d = distant_light_diffuse * ruby_diffuse;
color4 pillar_s = distant_light_specular * ruby_specular;
GLfloat pillar_sh = ruby_shininness;

//Products2 wall
color4 wall_a = distant_light_ambient * obsidian_ambient;
color4 wall_d = distant_light_diffuse * obsidian_diffuse;
color4 wall_s = distant_light_specular * obsidian_specular;
GLfloat wall_sh = obsidian_shininness;

//Products3 hill
color4 hill_a = distant_light_ambient * green_ambient;
color4 hill_d = distant_light_diffuse * green_diffuse;
color4 hill_s = distant_light_specular * green_specular;
GLfloat hill_sh = green_shininness;

//Products4 snow
color4 snow_a = distant_light_ambient * white_ambient;
color4 snow_d = distant_light_diffuse * white_diffuse;
color4 snow_s = distant_light_specular * white_specular;
GLfloat snow_sh = white_shininness;

//Products5 gold
color4 gold_a = distant_light_ambient * gold_ambient;
color4 gold_d = distant_light_diffuse * gold_diffuse;
color4 gold_s = distant_light_specular * gold_specular;
GLfloat gold_sh = gold_shininness;

//Products6 yellow
color4 yellow_a = distant_light_ambient * yellow_ambient;
color4 yellow_d = distant_light_diffuse * yellow_diffuse;
color4 yellow_s = distant_light_specular * yellow_specular;
GLfloat yellow_sh = yellow_shininness;

//Products7 gold inside
color4 goldi_a = point_light_ambient * gold_ambient;
color4 goldi_d = point_light_diffuse * gold_diffuse;
color4 goldi_s = point_light_specular * gold_specular;
GLfloat goldi_sh = gold_shininness;

//Products8 ruby inside
color4 rubyi_a = point_light_ambient * ruby_ambient;
color4 rubyi_d = point_light_diffuse * ruby_diffuse;
color4 rubyi_s = point_light_specular * ruby_specular;
GLfloat rubyi_sh = ruby_shininness;

//Products8 tree
color4 tree_a = point_light_ambient * trunk_ambient;
color4 tree_d = point_light_diffuse * trunk_diffuse;
color4 tree_s = point_light_specular * trunk_specular;
GLfloat tree_sh = trunk_shininness;

//Products9 car
color4 car_a = distant_light_ambient * ruby_ambient;
color4 car_d = distant_light_diffuse * ruby_diffuse;
color4 car_s = distant_light_specular * ruby_specular;
GLfloat car_sh = ruby_shininness;

//Products10 dark
color4 dark_a = color4( 0.0, 0.0, 0.0, 1.0 );
color4 dark_d = color4( 0.0, 0.0, 0.0, 1.0 );
color4 dark_s = color4( 0.0, 0.0, 0.0, 1.0 );
GLfloat dark_sh = 0.0;






