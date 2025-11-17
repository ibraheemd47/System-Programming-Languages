package bguspl.set.ex;

import bguspl.set.Env;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    Thread[] playersThreads = null;
    private final Table table;
    private final Player[] players;

    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;
    private Integer id;

    /**
     * True iff game should be terminated.
     */
    private volatile boolean terminate;
    public BlockingQueue<Integer> checkSets;
    /**
     * The time when the dealer needs to reshuffle the deck due to turn timeout.
     */
    private long reshuffleTime = Long.MAX_VALUE;

    public Dealer(Env env, Table table, Player[] players) {
        this.env = env;
        this.table = table;
        this.players = players;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
        checkSets = new LinkedBlockingQueue<>();
    }

    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        env.logger.info("thread " + Thread.currentThread().getName() + " starting.");
        long time = System.currentTimeMillis();
        playersThreads = new Thread[players.length];

        for (Player player : players) {
            playersThreads[player.id] = new Thread(player);
            playersThreads[player.id].start();
        }

        while (!shouldFinish()) {
            
            updateTimerDisplay(true);
            placeCardsOnTable();
            updateTimerDisplay(true);
            table.touchTable = false;
            if (env.config.hints) {
                table.hints();
            }
            timerLoop();
            table.touchTable = true;
            if(deck.size() == 0 && env.util.findSets(table.toList(), 1).size() == 0 || table.toList().size() == 0 && env.util.findSets(deck, 1).size() == 0) break;
            updateTimerDisplay(true);
            removeAllCardsFromTable();
            
        }
        removeAllCardsFromTable();
        terminate();
        announceWinners();

        env.logger.info("thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * The inner loop of the dealer thread that runs as long as the countdown did
     * not time out.
     */
    private void timerLoop() {
        while (!terminate && System.currentTimeMillis() < reshuffleTime) {
            sleepUntilWokenOrTimeout();
            updateTimerDisplay(false);
            removeCardsFromTable();
            if(env.util.findSets(table.toList(), 1).size() == 0 && deck.size()==0) break;
            placeCardsOnTable();
        }
    }

    /**
     * Called when the game should be terminated.
     */
    public void terminate() {
        // TODO implement
        terminate = true;
        for (int player = players.length - 1; player >= 0; player--) {
            players[player].terminate();
        }

    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private boolean shouldFinish() {
        List<Integer> all = new LinkedList<>();
        for (int i = 0; i < deck.size(); i++) {
            all.add(deck.get(i));
        }
        for (int i = 0; i < table.toList().size(); i++) {
            all.add(table.toList().get(i));
        }
        return terminate || env.util.findSets(all, 1).size() == 0;
    }//

    /**
     * Checks cards should be removed from the table and removes them.
     */
    // split to functions
    private void removeCardsFromTable() {
        // TODO implement
        try {
            if (id != null) {
                int[] slots = players[id].setArray();
                int[] cards = new int[slots.length];
                for (int i = 0; i < slots.length; i++) {
                    if(table.slotToCard[slots[i]] == null){
                        players[id].ans.put(Answers.NO_CARDS);
                        players[id].tokens.remove((Integer)slots[i]);
                        break;
                    }
                    cards[i] = table.slotToCard[slots[i]];
                }
                if (cards.length < env.config.featureSize) {
                    players[id].ans.put(Answers.CHANGES_IN_SET);
                } else if (env.util.testSet(cards) == false) {
                    players[id].ans.put(Answers.PENALTY);
                } else {
                    for (Player p : players) {
                        for (int i = 0; i < cards.length; i++) {
                            p.tokens.remove((Integer) slots[i]);
                            table.removeToken(p.id, slots[i]);
                        }
                    }
                    players[id].ans.put(Answers.SCORE);
                    // here remove card
                    for (int j = 0; j < cards.length; j++) {
                        table.removeCard(slots[j]);
                    }
                    updateTimerDisplay(true);
                }
            }
        } catch (InterruptedException e) {}
        
    }

    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {
        // TODO implement
        table.placeCardOnTable(deck);

    }

    /**
     * Sleep for a fixed amount of time or until the thread is awakened for some
     * purpose.
     */
    private void sleepUntilWokenOrTimeout() {
        // TODO implement

        try {
            if (reshuffleTime - System.currentTimeMillis() > env.config.turnTimeoutWarningMillis) {
                id = checkSets.poll(1000, TimeUnit.MILLISECONDS);
            } else {
                id = checkSets.poll(10, TimeUnit.MILLISECONDS);
            }
        } catch (InterruptedException e) {
        }
    }

    /**
     * Reset and/or update the countdown and the countdown display.
     */
    private void updateTimerDisplay(boolean reset) {
        // TODO implement
        if (reset) {
            
            reshuffleTime = System.currentTimeMillis() + env.config.turnTimeoutMillis;
            env.ui.setCountdown(env.config.turnTimeoutMillis, false);
        } else {
            long timeLeft = reshuffleTime - System.currentTimeMillis();
            
            env.ui.setCountdown(timeLeft, timeLeft < env.config.turnTimeoutWarningMillis);
        }
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        // TODO implement
        for (int i = 0; i < env.config.tableSize; i++) {
            Integer card = table.slotToCard[i];
            deck.add(card);
            table.removeCard(i);
        }
        Collections.shuffle(deck);
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() {
        // TODO implement
        int maxScore = 0;
        int numWin = 0;
        for (int i = 0; i < players.length; i++) {
            if (players[i].score() > maxScore)
                maxScore = players[i].score();
        }
        for (Player player : players) {
            if (player.score() == maxScore)
                numWin++;
        }
        int[] playersId = new int[numWin];
        int i = 0;
        for (Player player : players) {
            if (player.score() == maxScore){
                playersId[i] = player.id;
                i++;
            }
        }
        env.ui.announceWinner(playersId);
    }
}
