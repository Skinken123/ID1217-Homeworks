public class Bathroom {
    private int nrOfWaitingMen = 0, nrOfWaitingWomen = 0, nrOfPeopleInBathroom = 0;
    private char currentGender = 'N'; // 'M' for men, 'W' for women, 'N' for none 
 
    public synchronized void enterBathroom(char gender, int id) throws InterruptedException {
 
    }
 
    public synchronized void leaveBathroom(char gender, int id) {
 
    }
 }