#pragma once

std::string RubberDucky = "data:image/jpeg;base64,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0WC5XApdvvVkR+/6U8R+9OwrlUJ704J71aCY704LjvRYLkAj9/0qQJ71JilpiGhfelxS0UAFFFFABRRRQAUUUUAFFFFAH//Z/+EOoWh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8APD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iWE1QIENvcmUgNC40LjAtRXhpdjIiPiA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0iIiB4bWxuczppcHRjRXh0PSJodHRwOi8vaXB0Yy5vcmcvc3RkL0lwdGM0eG1wRXh0LzIwMDgtMDItMjkvIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZUV2ZW50IyIgeG1sbnM6cGx1cz0iaHR0cDovL25zLnVzZXBsdXMub3JnL2xkZi94bXAvMS4wLyIgeG1sbnM6R0lNUD0iaHR0cDovL3d3dy5naW1wLm9yZy94bXAvIiB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iIHhtbG5zOnhtcD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLyIgeG1wTU06RG9jdW1lbnRJRD0iZ2ltcDpkb2NpZDpnaW1wOmE1ZDFkZDc2LTEyYWMtNGZkOC1hZjBkLTdhNDY2ODY3MTEzZiIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo0NDRhNzFhNC1lMjE0LTRhMmUtODNiNC04MjM3OGM5ZTI0OTAiIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDo4Y2ZiZGVlOS0wNTZiLTRjNjMtODk3NC1iOWIzNjZlYTk3YjQiIEdJTVA6QVBJPSIyLjAiIEdJTVA6UGxhdGZvcm09IldpbmRvd3MiIEdJTVA6VGltZVN0YW1wPSIxNjg4MTI4MjA4MDgxODA4IiBHSU1QOlZlcnNpb249IjIuMTAuMjIiIGRjOkZvcm1hdD0iaW1hZ2UvanBlZyIgeG1wOkNyZWF0b3JUb29sPSJHSU1QIDIuMTAiPiA8aXB0Y0V4dDpMb2NhdGlvbkNyZWF0ZWQ+IDxyZGY6QmFnLz4gPC9pcHRjRXh0OkxvY2F0aW9uQ3JlYXRlZD4gPGlwdGNFeHQ6TG9jYXRpb25TaG93bj4gPHJkZjpCYWcvPiA8L2lwdGNFeHQ6TG9jYXRpb25TaG93bj4gPGlwdGNFeHQ6QXJ0d29ya09yT2JqZWN0PiA8cmRmOkJhZy8+IDwvaXB0Y0V4dDpBcnR3b3JrT3JPYmplY3Q+IDxpcHRjRXh0OlJlZ2lzdHJ5SWQ+IDxyZGY6QmFnLz4gPC9pcHRjRXh0OlJlZ2lzdHJ5SWQ+IDx4bXBNTTpIaXN0b3J5PiA8cmRmOlNlcT4gPHJkZjpsaSBzdEV2dDphY3Rpb249InNhdmVkIiBzdEV2dDpjaGFuZ2VkPSIvIiBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOjdjNTRiMzdlLTJmOTAtNDFhNS1iZTVkLTQ3NTI2MDQzMTE5ZSIgc3RFdnQ6c29mdHdhcmVBZ2VudD0iR2ltcCAyLjEwIChXaW5kb3dzKSIgc3RFdnQ6d2hlbj0iMjAyMy0wNi0zMFQxNDozMDowOCIvPiA8L3JkZjpTZXE+IDwveG1wTU06SGlzdG9yeT4gPHBsdXM6SW1hZ2VTdXBwbGllcj4gPHJkZjpTZXEvPiA8L3BsdXM6SW1hZ2VTdXBwbGllcj4gPHBsdXM6SW1hZ2VDcmVhdG9yPiA8cmRmOlNlcS8+IDwvcGx1czpJbWFnZUNyZWF0b3I+IDxwbHVzOkNvcHlyaWdodE93bmVyPiA8cmRmOlNlcS8+IDwvcGx1czpDb3B5cmlnaHRPd25lcj4gPHBsdXM6TGljZW5zb3I+IDxyZGY6U2VxLz4gPC9wbHVzOkxpY2Vuc29yPiA8L3JkZjpEZXNjcmlwdGlvbj4gPC9yZGY6UkRGPiA8L3g6eG1wbWV0YT4gICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICA8P3hwYWNrZXQgZW5kPSJ3Ij8+/+ICsElDQ19QUk9GSUxFAAEBAAACoGxjbXMEMAAAbW50clJHQiBYWVogB+cABgAeAAwAFAAVYWNzcE1TRlQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPbWAAEAAAAA0y1sY21zAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANZGVzYwAAASAAAABAY3BydAAAAWAAAAA2d3RwdAAAAZgAAAAUY2hhZAAAAawAAAAsclhZWgAAAdgAAAAUYlhZWgAAAewAAAAUZ1hZWgAAAgAAAAAUclRSQwAAAhQAAAAgZ1RSQwAAAhQAAAAgYlRSQwAAAhQAAAAgY2hybQAAAjQAAAAkZG1uZAAAAlgAAAAkZG1kZAAAAnwAAAAkbWx1YwAAAAAAAAABAAAADGVuVVMAAAAkAAAAHABHAEkATQBQACAAYgB1AGkAbAB0AC0AaQBuACAAcwBSAEcAQm1sdWMAAAAAAAAAAQAAAAxlblVTAAAAGgAAABwAUAB1AGIAbABpAGMAIABEAG8AbQBhAGkAbgAAWFlaIAAAAAAAAPbWAAEAAAAA0y1zZjMyAAAAAAABDEIAAAXe///zJQAAB5MAAP2Q///7of///aIAAAPcAADAblhZWiAAAAAAAABvoAAAOPUAAAOQWFlaIAAAAAAAACSfAAAPhAAAtsRYWVogAAAAAAAAYpcAALeHAAAY2XBhcmEAAAAAAAMAAAACZmYAAPKnAAANWQAAE9AAAApbY2hybQAAAAAAAwAAAACj1wAAVHwAAEzNAACZmgAAJmcAAA9cbWx1YwAAAAAAAAABAAAADGVuVVMAAAAIAAAAHABHAEkATQBQbWx1YwAAAAAAAAABAAAADGVuVVMAAAAIAAAAHABzAFIARwBC/9sAQwAQCwwODAoQDg0OEhEQExgoGhgWFhgxIyUdKDozPTw5Mzg3QEhcTkBEV0U3OFBtUVdfYmdoZz5NcXlwZHhcZWdj/9sAQwEREhIYFRgvGhovY0I4QmNjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2Nj/8IAEQgAQABAAwERAAIRAQMRAf/EABoAAAIDAQEAAAAAAAAAAAAAAAAEAQMFAgb/xAAZAQADAQEBAAAAAAAAAAAAAAAAAQIDBAX/2gAMAwEAAhADEAAAAfQABAV89RRdtIAAACuVr8GlGq1u7HtgAEBg8vbS6ay5tXsyupAAQGXzbJzp0o0bjrBt9+QxTh0Si8/pbeKtkslP9+XTM3z9ZCGcsrpgtLsyZpYvDtKICGVUdazsbZ3MolqTXKY0zUt0pD//xAAiEAABBAEEAgMAAAAAAAAAAAACAAEDERIEECAhFDMTMTL/2gAIAQEAAQUC3c2QmzvxmLq00loXseEzlkJ0vtRevhOTZ4irZkEwuOb7kdqZrFmJfD0AsCtRes/zava1avqGTMVk17vILII5JVHG0YqSEJE+lXjEvEtBpox3/8QAHREAAQQDAQEAAAAAAAAAAAAAAQACEBIRIDEDUf/aAAgBAwEBPwGQ1FuNmtJju3nxEBy4j3VmQi8xX6i0S0TmT1Dc7kyDhXV1dWM//8QAIhEAAgIBBAIDAQAAAAAAAAAAAAECEQMQEiAhEzIEIjFR/9oACAECAQE/AdZ/Iivwx5VPlOS9Tb2bNrE7XFIcRvvsj+cZSTf1LZRPLtXSI5pX3rlybukYnTOjyfwk3Ioh6k/VlFFFFFC0argosUBKtGrPGjxo2I2rX//EACIQAAEEAgEEAwAAAAAAAAAAAAEAEBEgAiExIjNBUWFxof/aAAgBAQAGPwKkWgGC0hA12+NdKWjHxT4Wm2XCN+Iho4r6CgNsLpzK7n4t5lcT9v8A/8QAHxAAAgICAwEBAQAAAAAAAAAAAAERMRAhQVFhIKFx/9oACAEBAAE/IcpVs4Uf1DGs5ZXRB2Hti+VHgTik9n4vlb2V9kj5HOrwhS1TYlOZkTlJqnibCD5rIYcJMXJyKrkqyW00j6ySPE4SYuJcCB0SPZ08bNcmoW34ME4H9gY7C7RDT/uz1iNqbHcEox//2gAMAwEAAgADAAAAEAIYAAI0QBEWABKbMp3WDpiTUhr7zYiMwP/EABwRAQACAwEBAQAAAAAAAAAAAAEAERAhMSBBcf/aAAgBAwEBPxDKvY3psAiJ2WCJTXmpchFROJ28oIiIu5u3FDWadsGpZg47To8rqMFODnioJFvC8QX0n4lopn//xAAfEQEAAgMAAwADAAAAAAAAAAABABEQITEgQVFxsdH/2gAIAQIBAT8QyjWzNIlPkxs/n+QbSWqeyofELp7G6diNupz8ORGqaagl3HIke6uCJZh16v3BoYo2x30xhvOVy2j5HDtDEWajE1GOnWpqam/UV2fKAKMDCmIDkAz/AP/EACMQAQACAgEEAwADAAAAAAAAAAEAESExURBBYaEgcYGxwdH/2gAIAQEAAT8Q63Ib+pYA9kdPysgZyVMcgrjEA3FOLKSeG5fjnBDbxKnNHEdFEyuY/CLRbLVgUNBPwwtRb8/qIAScIIUwU7ZZryyeITqwsejo9D3B4Wt1Mmb6hpRMyBqIgadrEtE8kr1eIlWw4TrKjdEu7xynCn8zNAItiEWJYmo2hSpKnsxhoRY/RQ7eMug8cwIHlW16W7X9ZiarwIs3R4P+y3g+5gjYKHdX61AFBXT/2Q==";