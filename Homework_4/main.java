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

