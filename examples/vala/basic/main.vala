/**
 * This example creates a database of students.
 * It generates some random numbers as their 'results'
 * and then updates the database with grades.
 *
 */


// adds the students to the database
void add_student(Spino.Collection collection) {
    Json.Builder builder = new Json.Builder ();
    builder.begin_object ();

    // get a name from stdint
    builder.set_member_name ("name");
    stdout.printf("Enter a student name: ");
    string? name = stdin.read_line ();
    builder.add_string_value (name);


    // generate 5 random numbers and add them 
    // to the document as an array
    builder.set_member_name("results");
    builder.begin_array ();
    for(int i = 0; i < 5; i++) {
        builder.add_int_value(Random.int_range(30,100));
    }
    builder.end_array ();
    builder.end_object ();


    // turn the DOM into a string
    Json.Generator generator = new Json.Generator ();
    Json.Node root = builder.get_root ();
    generator.set_root (root);

    // append the document string to the collection
    collection.append(generator.to_data(null));
}

void calculate_grades(Spino.Collection student_collection) {
    // the query {} will resurn every document in the collection
    // we're doing this for every student
    var cursor = student_collection.find("{}");
    var docstr = "";

    // while the cursor isn't returning empty strings
    while(cursor.has_next() == true) {
        docstr = cursor.next();
        // parse the document
        var parser = new Json.Parser ();
        parser.load_from_data(docstr, -1);
        //get root object
        var root_object = parser.get_root().get_object();
        //get the students name
        var name = root_object.get_string_member("name");
        var results_arr = root_object.get_array_member("results");
        var avg = 0;
        var count = 0;
        string grade = "F";

        stdout.printf("Student name: %s\n", name);

        // iterate over their results
        stdout.printf("Results:\n\t");
        foreach(var r in results_arr.get_elements()) {
            stdout.printf("%i ", (int)r.get_int());
            avg += (int)r.get_int();
            count++;
        }
        //calculate the grade
        avg /= count;
        if(avg > 50) {
            grade = "C";
        }
        if(avg > 75) {
            grade = "B";
        }
        if(avg > 90) {
            grade = "A";
        }


        // update the document with the grade
        student_collection.update(
            // this query uses the name as the index
            "{name: \"%s\"}".printf(Spino.escape(name)),
            // this is the document to be merged in
            // (its just the grade)
            "{\"grade\": \"%s\"}".printf(Spino.escape(grade))
            );

        stdout.printf("\n\n");
    }


}


public static int main() {
    var db = new Spino.Database();

    // get the student collection
    // if the collection doesn't exist, it is created.
    var student_collection = db.get_collection("students");
    // create the index
    student_collection.create_index("name");

    // insert some student documents
    for(int i = 0; i < 3; i++) {
        add_student(student_collection);
    }

    // calculate some grades for them
    calculate_grades(student_collection);

    // save the data to a file
    db.save("data.db");
    return 0;
}


