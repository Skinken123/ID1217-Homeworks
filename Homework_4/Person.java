import java.util.Random;

public class Person extends Thread {
    private Bathroom bathroom;
    private char gender;
    private int id;
    private Random random = new Random();

    public Person(Bathroom bathroom, char gender, int id) {
        this.bathroom = bathroom;
        this.gender = gender;
        this.id = id;
    }

    @Override
    public void run() {

    }
}