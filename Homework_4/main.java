import java.util.Random;

class Bathroom {
   private int nrOfWaitingMen = 0, nrOfWaitingWomen = 0, nrOfPeopleInBathroom = 0;
   private char currentGender = 'N'; // 'M' for men, 'W' for women, 'N' for none 

   public synchronized void enterBathroom(char gender, int id) throws InterruptedException {

   }

   public synchronized void leaveBathroom(char gender, int id) {

   }
}

class Person extends Thread {
    private Bathroom bathroom;
    private char gender;
    private int id;
    private Random random = new Random();

    public Person(Bathroom bathroom, char gender, int id) {
        this.bathroom = bathroom;
        this.gender = gender;
        this.id = id;
    }
}

@Override
public void run() {

}

public class Main {
    public static void main(String[] args) {
        final int MAX_WORKERS = 16;
        Bathroom bathroom = new Bathroom();

        for (int i = 0; i < MAX_WORKERS / 2; i++) {
            new Person(bathroom, 'M', i).start();
            new Person(bathroom, 'W', i).start();
        }
    }
}

