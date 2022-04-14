/**
 * This is the Vala version of the solar system test
 *
 * Results on my PC
 *
 * Script time: 280ms
 * Native time: 1162ms
 *
 */


void generate_random_solarsystems(Spino.Collection pc) {
    string[] star_types = {"solar", "hotblue", "reddwarf", "redgiant", "whitedwarf", "neutron"};
    string[] planet_types = {"gas", "rock"};

    var doc = pc.create_node();
    var arr = pc.create_node();
    var planet_doc = pc.create_node();

    for(int i = 0; i < 1000000; i++) {
        doc.set_object();

        var star_name = GLib.Random.int_range(0, 100000).to_string();
        doc.add_string_member("name", star_name);
        doc.add_string_member("type", star_types[GLib.Random.int_range(0, 6)]);
        doc.add_int_member("dist", GLib.Random.int_range(100, 100000));

        arr.set_array();

        var n_planets = GLib.Random.int_range(0, 6);
        for(int j = 0; j < n_planets; j++) {
            planet_doc.set_object();
            planet_doc.add_string_member("type", planet_types[GLib.Random.int_range(0, 2)]);
            planet_doc.add_double_member("dist", (double)GLib.Random.double_range(0.1, 10.0));
            arr.append(planet_doc);
        }

        doc.add_member("planets", arr);
        pc.append_node(doc);
    }
}

struct planetResult {
    public string star;
    public int planet;
    public int dist;
}

string nativeValueFoo(Spino.Collection pc) {
    var cur = pc.find("{}");
    var list = new GLib.SList<planetResult?>();
    while(cur.has_next()) {
        var doc = cur.next_view();

        var planets_arr = doc.get_member("planets");
        var iter = planets_arr.begin();
        int count = 0;
        while(iter.compare(planets_arr.end()) == false) {
            var planet_doc = iter.get_view();
            if(planet_doc.get_string_member("type") == "rock") {
                var dist = planet_doc.get_double_member("dist");
                if((dist > 3.2) && (dist < 3.8)) {
                    planetResult pr = planetResult();
                    pr.star = doc.get_string_member("name");
                    pr.planet = count;
                    pr.dist = doc.get_int_member("dist");
                    list.insert_sorted(pr, (a, b) => {
                        if(a.dist > b.dist) { return 1; }
                        else { return -1; }
                    });
                    if(list.length() > 5) {
                        list.remove_link(list.last());
                    }
                }
            }
            count++;
            iter.next();
        }
    }

    var json = new GLib.StringBuilder();
    json.append("[");
    list.@foreach((item) => {
        json.append("{\"planet\":");
        json.append(item.planet.to_string());

        json.append(",\"dist\":");
        json.append(item.dist.to_string());

        json.append(",\"star\":\"");
        json.append(item.star);

        json.append("\"},");
    });
    json.erase(json.len-1, 1);
    json.append("]");

    return json.str;


}


string nativeFoo(Spino.Collection pc) {
    var cur = pc.find("{}");
    var parser = new Json.Parser();
    var list = new GLib.SList<planetResult?>();
    while(cur.has_next()) {
        parser.load_from_data(cur.next());

        var node = parser.get_root().get_object();
        var planets = node.get_array_member("planets");
        int count = 0;
        foreach(unowned Json.Node planet in planets.get_elements()) {
            var dist = planet.get_object().get_double_member("dist");
            if(planet.get_object().get_string_member("type") == "rock") {
                if((dist > 3.2) && (dist < 3.8)) {
                    planetResult pr = planetResult();
                    pr.star = node.get_string_member("name");
                    pr.planet = count;
                    pr.dist = (int)node.get_int_member("dist");
                    list.insert_sorted(pr, (a, b) => {
                        if(a.dist > b.dist) { return 1; }
                        else { return -1; }
                    });
                    if(list.length() > 5) {
                        list.remove_link(list.last());
                    }
                }
            }
            count++;
        }
    }

    var json = new GLib.StringBuilder();
    json.append("[");
    list.@foreach((item) => {
        json.append("{\"planet\":");
        json.append(item.planet.to_string());

        json.append(",\"dist\":");
        json.append(item.dist.to_string());

        json.append(",\"star\":\"");
        json.append(item.star);

        json.append("\"},");
    });
    json.erase(json.len-1, 1);
    json.append("]");

    return json.str;
}


string run_script(Spino.Collection pc) {
    string script = """

        goldilocksZoned <- [];
    function result(ss) {
        local count = 0;
        foreach(p in ss.planets) {
            if(p.type == "rock") {
                if((p.dist > 3.2) && 
                   (p.dist < 3.8)) {
                    local p = {
                        "star": ss.name,
                        "planet": count,
                        "dist": ss.dist
                    }
                    goldilocksZoned.append(p);
                }
            }
            count++;
        }
    }

    function finalize() {
        goldilocksZoned.sort(@(a, b) a.dist <=> b.dist);
        if(goldilocksZoned.len() > 5) {
            goldilocksZoned.resize(5);
        }
        return goldilocksZoned;
    }


    """;

    return pc.find("{}").run_script(script);
}

public static int main() {
    var db = new Spino.Database();

    var solarsystems = db.get_collection("solarsystems");
    //solarsystems.create_index("dist");


    int64 msec = GLib.get_real_time () / 1000;
    generate_random_solarsystems(solarsystems);
    int64 msec2 = GLib.get_real_time() / 1000;
    print("Generation time: %i\n", (int)(msec2-msec));



    msec = GLib.get_real_time () / 1000;
    var result = run_script(solarsystems);
    print("%s\n".printf(result));
    msec2 = GLib.get_real_time() / 1000;
    print("Script time: %i\n", (int)(msec2-msec));

    msec = GLib.get_real_time() / 1000;
    result = nativeFoo(solarsystems);
    print("%s\n".printf(result));
    msec2 = GLib.get_real_time() / 1000;
    print("Native time: %i\n", (int)(msec2-msec));

    msec = GLib.get_real_time() / 1000;
    result = nativeValueFoo(solarsystems);
    print("%s\n".printf(result));
    msec2 = GLib.get_real_time() / 1000;
    print("Native time: %i\n", (int)(msec2-msec));



    return 0;
}


