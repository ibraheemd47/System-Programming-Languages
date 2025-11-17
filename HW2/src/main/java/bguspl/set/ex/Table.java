package bguspl.set.ex;

import bguspl.set.Env;

import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.stream.Collectors;

/**
 * This class contains the data that is visible to the player.
 *
 * @inv slotToCard[x] == y iff cardToSlot[y] == x
 */
public class Table {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Mapping between a slot and the card placed in it (null if none).
     */
    protected final Integer[] slotToCard; // card per slot (if any)

    /**
     * Mapping between a card and the slot it is in (null if none).
     */
    protected final Integer[] cardToSlot; // slot per card (if any)

    // new members of the class

    public BlockingQueue<Player> playersQue = new LinkedBlockingDeque<Player>(); // FIFO;
    Object[] keysToSlots;
    boolean touchTable = true;
    // list of cards on the table
    protected final Integer[] cardsOnTable; 
    /**
     * Constructor for testing.
     *
     * @param env        - the game environment objects.
     * @param slotToCard - mapping between a slot and the card placed in it (null if
     *                   none).
     * @param cardToSlot - mapping between a card and the slot it is in (null if
     *                   none).
     */
    public Table(Env env, Integer[] slotToCard, Integer[] cardToSlot) {

        this.env = env;
        this.slotToCard = slotToCard;
        this.cardToSlot = cardToSlot;
        keysToSlots = new Object[env.config.tableSize];
        this.cardsOnTable = new Integer[env.config.tableSize];
        for (int i = 0; i < env.config.tableSize; i++) {
            keysToSlots[i] = new Object();
        }
    }

    /**
     * Constructor for actual usage.
     *
     * @param env - the game environment objects.
     */
    public Table(Env env) {

        this(env, new Integer[env.config.tableSize], new Integer[env.config.deckSize]);
    }

    /**
     * This method prints all possible legal sets of cards that are currently on the
     * table.
     */
    public void hints() {
        List<Integer> deck = Arrays.stream(slotToCard).filter(Objects::nonNull).collect(Collectors.toList());
        env.util.findSets(deck, Integer.MAX_VALUE).forEach(set -> {
            StringBuilder sb = new StringBuilder().append("Hint: Set found: ");
            List<Integer> slots = Arrays.stream(set).mapToObj(card -> cardToSlot[card]).sorted()
                    .collect(Collectors.toList());
            int[][] features = env.util.cardsToFeatures(set);
            System.out.println(
                    sb.append("slots: ").append(slots).append(" features: ").append(Arrays.deepToString(features)));
        });
    }

    /**
     * Count the number of cards currently on the table.
     *
     * @return - the number of cards on the table.
     */
    public int countCards() {
        int cards = 0;
        for (Integer card : slotToCard)
            if (card != null)
                ++cards;
        return cards;
    }

    /**
     * Places a card on the table in a grid slot.
     * 
     * @param card - the card id to place in the slot.
     * @param slot - the slot in which the card should be placed.
     *
     * @post - the card placed is on the table, in the assigned slot.
     */
    public void placeCard(int card, int slot) {
        try {
            Thread.sleep(env.config.tableDelayMillis);
        } catch (InterruptedException ignored) {
        }
        // TODO implement
        synchronized (keysToSlots[slot]) {
            slotToCard[slot] = card;
            cardToSlot[card] = slot;
            env.ui.placeCard(card, slot);
            cardsOnTable[slot] = card;
        }

    }

    /**
     * Removes a card from a grid slot on the table.
     * 
     * @param slot - the slot from which to remove the card.
     */
    public void removeCard(int slot) {
        try {
            Thread.sleep(env.config.tableDelayMillis);
        } catch (InterruptedException ignored) {
        }
        // TODO implement
        synchronized (keysToSlots[slot]) {
            if (slotToCard[slot] != null) {
                cardToSlot[slotToCard[slot]] = null;
                slotToCard[slot] = null;
                env.ui.removeTokens(slot);
                env.ui.removeCard(slot);
                cardsOnTable[slot] = null ;
            }
        }
    }
    public List<Integer> toList(){
        List<Integer> toRet = new LinkedList<>();
        for (int j = 0; j < cardsOnTable.length; j++) {
            if(cardsOnTable[j] != null){
                toRet.add(cardsOnTable[j]) ;
            }
        }
        return toRet ;
        
    }

    /**
     * Places a player token on a grid slot.
     * 
     * @param player - the player the token belongs to.
     * @param slot   - the slot on which to place the token.
     */
    // This function will be used by the dealer. (Player sets a token)
    public void placeToken(int player, int slot) {

        // }
        synchronized (keysToSlots[slot]) {
            Integer card = slotToCard[slot];
            if (card != null && cardToSlot[card] != null) // Slot has no card (Deck empty?) & card exists.
                env.ui.placeToken(player, slot);
        }

    }

    /**
     * Removes a token of a player from a grid slot.
     * 
     * @param player - the player the token belongs to.
     * @param slot   - the slot from which to remove the token.
     * @return - true iff a token was successfully removed.
     */
    // Reshuffe, found set, player remove.
    public boolean removeToken(int player, int slot) {
        // TODO implement
        synchronized (keysToSlots[slot]) {
            Integer card = slotToCard[slot];
            if (card != null && cardToSlot[card] != null) {
                env.ui.removeToken(player, slot);
                return true;
            }
            return false;
        }
    }

    /**
     * Placing the cards on table,
     * Don't touch the table.
     */
    // Each set found, reshuffling.
    public synchronized void placeCardOnTable(List<Integer> deck) {
        synchronized (keysToSlots) {
            Collections.shuffle(deck);
            for (int i = 0; i < env.config.tableSize; i++) {
                Integer card = slotToCard[i]; // Get a card if there is any on i'th place.
                if (card == null) {
                    if (deck.size() > 0 && deck.get(0) != null) {
                        card = deck.remove(0);
                        placeCard(card, i);
                    }
                }
            }
        }
    }

}
