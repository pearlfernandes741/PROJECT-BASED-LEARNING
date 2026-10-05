# Logger Testing

## Test 1 - CPU Message
Input:
`CPU instruction executed`

Result: Message received and stored successfully.

## Test 2 - Memory Message
Input:
`Memory allocated`

Result: Message received and stored successfully.

## Test 3 - Stack Message
Input:
`Stack PUSH performed`

Result: Message received and stored successfully.

## Test 4 - Queue Message
Input:
`Queue ENQUEUE performed`

Result: Message received and stored successfully.

## Test 5 - Error Message
Input:
`ERROR: Invalid instruction`

Result: Message received and stored successfully.

## Continuous Logging Test
Multiple messages were sent without restarting the logger.

Result: The logger successfully received and stored multiple messages continuously.