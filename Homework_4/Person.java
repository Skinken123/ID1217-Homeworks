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
        try {
            while (true) {
                Thread.sleep((random.nextInt(3) + 1) * 1000);
                bathroom.enterBathroom(gender, id);
                Thread.sleep((random.nextInt(2) + 2) * 1000);
                bathroom.leaveBathroom(gender, id);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}