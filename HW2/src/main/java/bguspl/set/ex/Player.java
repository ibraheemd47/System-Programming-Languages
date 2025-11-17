package bguspl.set.ex;

import java.util.LinkedList;
import java.util.List;
import java.util.Random;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import bguspl.set.Env;

/**
 * This class manages the players' threads and data
 *
 * @inv id >= 0
 * @inv score >= 0
 */
public class Player implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;

    /**
     * The id of the player (starting from 0).
     */
    public final int id;

    /**
     * The thread representing the current player.
     */
    private Thread playerThread;

    /**
     * The thread of the AI (computer) player (an additional thread used to generate
     * key presses).
     */
    private Thread aiThread;

    /**
     * True iff the player is human (not a computer player).
     */
    private final boolean human;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;

    /**
     * The current score of the player.
     */
    private int score;
    public BlockingQueue<Integer> playerTokens = new LinkedBlockingQueue<>();
    public BlockingQueue<Answers> ans = new LinkedBlockingQueue<>();
    public List<Integer> tokens = new LinkedList<>();
    public volatile boolean legalSet;
    public volatile boolean mustWait;
    public volatile boolean freezeMode;
    public volatile boolean checkPenalty;
    public volatile boolean waitAI;
    private Dealer dealer;
    public Integer[] myCard;

    /**
     * The class constructor.
     *
     * @param env    - the environment object.
     * @param dealer - the dealer object.
     * @param table  - the table object.
     * @param id     - the id of the player.
     * @param human  - true iff the player is a human player (i.e. input is provided
     *               manually, via the keyboard).
     */

    public Player(Env env, Dealer dealer, Table table, int id, boolean human) {
        this.env = env;
        this.table = table;
        this.id = id;
        this.human = human;
        this.dealer = dealer;
        myCard = new Integer[env.config.deckSize];
        for (int i = 0; i < myCard.length; i++) {
            myCard[i] = null;
        }
    }

    /**
     * The main player thread of each player starts here (main loop for the player
     * thread).
     */
    @Override
    public void run() {
        playerThread = Thread.currentThread();
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        if (!human)
            createArtificialIntelligence();

        while (!terminate) {
            /**
             * Enter when
             * Got 3 cards.
             * Dealer didn't check my set.
             * PlayersQueue has me on it.
             */
            try {
                Integer card = playerTokens.take();
                if (tokens.contains(card)) {
                    tokens.remove(card);
                    table.removeToken(id, card);
                } else {
                    if (tokens.size() < env.config.featureSize) {
                        tokens.add(card);
                        table.placeToken(id, card);
                        if (tokens.size() == env.config.featureSize) {
                            dealer.checkSets.put(id);
                            Answers a = ans.take();
                            if (a == Answers.SCORE) {
                                point();
                            } else if (a == Answers.PENALTY) {
                                penalty();
                            }
                            ans.clear();
                        }
                    }
                }
            } catch (InterruptedException e) {
            }
        }
        if (!human)
            try {
                aiThread.join();
            } catch (InterruptedException ignored) {
            }
        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }

    
    /**
     * Creates an additional thread for an AI (computer) player. The main loop of
     * this thread repeatedly generates
     * key presses. If the queue of key presses is full, the thread waits until it
     * is not full.
     */
    private void createArtificialIntelligence() {
        // note: this is a very, very smart AI (!)
        aiThread = new Thread(() -> {
            env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
            while (!terminate) {
                Random rand = new Random();
                int slot = rand.nextInt(env.config.tableSize);
                keyPressed(slot);
                try{
                    Thread.sleep(1);
                } catch(InterruptedException e){}
               
            }
            env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
        }, "computer-" + id);
        aiThread.start();
    }

   

   

    public int[] setArray() {
        int[] arr = new int[tokens.size()];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = tokens.get(i);
        }
        return arr;
    }

    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        terminate = true;
        playerThread.interrupt();
        try{
            playerThread.join();
        }catch(InterruptedException e){}
    }

    /**
     * This method is called when a key is pressed.
     *
     * @param slot - the slot corresponding to the key pressed.
     */
    /**
     * AI & INPUT MANAGER FUNCTION.
     * If I have token on the table, remove it and update the status of checked
     * after getting penalty.
     * If I don't have the token on the card, add the token to the card, MyCard,
     * playerTokens.
     * 
     * @param slot
     */
    public void keyPressed(int slot) {
        // TODO implement
        if (!table.touchTable) {
            if (!freezeMode) {
                try {
                    playerTokens.put(slot);
                } catch (InterruptedException e) {
                }
            }
        }
    }

    /**
     * Award a point to a player and perform other related actions.
     *
     * @post - the player's score is increased by 1.
     * @post - the player's score is updated in the ui.
     */
    public void point() {
        // TODO implement
    
        int ignored = table.countCards(); // this part is just for demonstration in the unit tests
        env.ui.setScore(id, ++score);
        
    

        for (long i = env.config.pointFreezeMillis; i >= 0; i = i - 500) {
            try {
                if (i < 500)
                    Thread.sleep(i);
                else
                    Thread.sleep(500);
                env.ui.setFreeze(id, i);
            } catch (Exception e) {
            }
        }
        env.ui.setFreeze(id, 0);

    }

    /**
     * Penalize a player and perform other related actions.
     */
    public void penalty() {

        for (long i = env.config.penaltyFreezeMillis; i >= 0; i = i - 500) {
            try {
                if (i < 500){Thread.sleep(i);}
                else{ Thread.sleep(500);}
                env.ui.setFreeze(id, i);
            } catch (Exception e) {
            }
        }
        env.ui.setFreeze(id, 0);
    }

    public int score() {
        return score;
    }
}
