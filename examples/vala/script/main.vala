/**
*/


void generate_random_solarsystems(Spino.Collection pc) {
    string[] star_types = {"solar", "hotblue", "reddwarf", "redgiant", "whitedwarf", "neutron"};
    string[] planet_types = {"gas", "rock"};

    for(int i = 0; i < 1000; i++) {
        Json.Builder builder = new Json.Builder();
        builder.begin_object();
        builder.set_member_name("star_name");
        var star_name = GLib.Random.int_range(0, 100000).to_string();
        builder.add_string_value(star_name);

        builder.set_member_name("star_type");
        var star_type = star_types[GLib.Random.int_range(0, 6)];
        builder.add_string_value(star_type);

        builder.set_member_name("distance_from_earth");
        var distance_from_earth = GLib.Random.double_range(100, 100000);
        builder.add_double_value(distance_from_earth);

        builder.set_member_name("planets");
        builder.begin_array();
        var n_planets = GLib.Random.int_range(0, 6);
        for(int j = 0; j < n_planets; j++) {
            builder.begin_object();
            builder.set_member_name("planet_type");
            var planet_type = planet_types[GLib.Random.int_range(0, 2)];
            builder.add_string_value(planet_type);

            builder.set_member_name("distance_from_star");
            var distance_from_star = GLib.Random.double_range(0.1, 10.0);
            builder.add_double_value(distance_from_star);
            builder.end_object();
        }
        builder.end_array();
        builder.end_object();

        Json.Generator generator = new Json.Generator ();
        Json.Node root = builder.get_root ();
        generator.set_root (root);

        string str = generator.to_data (null);
        pc.append(str);
    }
}

string run_script(Spino.Collection pc, string query) {
    string script = """

goldilocksZoned <- [];
function result(solarsystem) {
    try {
    local count = 0;
    foreach(planet in solarsystem.planets) {
        if(planet.planet_type == "rock") {
            if((planet.distance_from_star > 3.2) && 
                (planet.distance_from_star < 3.8)) {
                local p = {
                    "star": solarsystem.star_name,
                    "planet": count,
                    "distance_from_earth": solarsystem.distance_from_earth
                }
                goldilocksZoned.append(p);
            }
        }
        count++;
    }
    }
    catch(err) {
        print(err);
    }
}

function finalize() {
    goldilocksZoned.sort(@(a, b) a.distance_from_earth <=> b.distance_from_earth);
    if(goldilocksZoned.len() > 5) {
        goldilocksZoned.resize(5);
    }
    return goldilocksZoned;
}


""";

    return pc.find(query).run_script(script);
}

public static int main() {
    var db = new Spino.Database();

    var solarsystems = db.get_collection("solarsystems");

    generate_random_solarsystems(solarsystems);

    var result = run_script(solarsystems, ("{}"));
    print(result);

    db.save("solarsystems.db");
    return 0;
}


