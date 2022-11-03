/**
 *
 */


void add_many_entries(Spino.Collection col)
{
    for(int i = 0; i < 1000000; i++) {
        col.append("{\"i\":" + i.to_string() + "}");
    }
}

int search(Spino.Collection col) 
{
    var view = col.find_one("{i:45432}");
    int len = view.length;
    len++;
    return 1;
}

public static int main() {
    var db = new Spino.Database();

    var testcol = db.get_collection("test");

    int64 msec = GLib.get_real_time () / 1000;
    add_many_entries(testcol);
    int64 msec2 = GLib.get_real_time() / 1000;
    print("Append Time: %ims\n", (int)(msec2-msec));


    testcol.create_index("i");
    int64 usec = GLib.get_real_time ();
    for(int i = 0; i < 10000; i++) {
        search(testcol);
    }
    int64 usec2 = GLib.get_real_time();
    print("Indexed Search Time: %ius\n", (int)(usec2-usec));

    testcol.drop_index("i");
    usec = GLib.get_real_time ();
    search(testcol);
    usec2 = GLib.get_real_time();
    print("Unindexed Search Time: %ius\n", (int)(usec2-usec));

    return 0;
}


